#include "TeamSpeak_VoiceClientComponent.h"

#include "TeamSpeak_SDKSubsystem.h"
#include "TeamSpeakFunctionLibrary.h"
#include "audiobridge/TeamSpeak_AudioBridge.h"
#include "audiobridge/TeamSpeak_SynthComponent.h"
#include <teamspeak/public_errors.h>
#include <teamspeak/clientlib.h>

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

// SDK's virtual capture/playback mode used for app-provided custom audio devices
// (the plugin's UTeamSpeak_AudioBridge registers under this mode). When custom is
// registered, the SDK may report it as the default capture mode — ResolveCaptureDeviceMode
// uses this constant to detect and skip past that case.
static constexpr const TCHAR* CustomModeId = TEXT("custom");

UTeamSpeak_VoiceClientComponent::UTeamSpeak_VoiceClientComponent()
{
    // Tick exists solely to poll the PTT hotkey (see TickComponent). It starts disabled and is
    // switched on only while PTT is enabled (RefreshTickEnabled), so a component that never uses
    // PTT never ticks — nothing here runs unless the consumer asks for it.
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTeamSpeak_VoiceClientComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Only reachable while PTT is enabled (the tick is disabled otherwise). Mic-level metering is
    // pull-based via GetCurrentInputLevel — this component does no per-frame audio polling.
    if (!bPTTEnabled || HandlerID == 0)
    {
        return;
    }

    const APlayerController* PC = GetOwningPC();
    if (!PC || !PC->IsLocalController() || !PTTKey.IsValid())
    {
        return;
    }

    // If PTT was enabled before the handler existed (toggle on pre-connect), SetPTTEnabled's gate call
    // was a no-op. Assert it once now so the mic starts silent without needing a priming key press.
    if (!bPTTGateApplied)
    {
        bPTTGateApplied = true;
        SetPushToTalkActive(false);
    }

    // Poll raw key state (UPlayerInput), edge-detect, gate transmission. We poll rather than bind an
    // Enhanced Input action because programmatically-created IA event bindings did not fire reliably.
    const bool bDown = PC->IsInputKeyDown(PTTKey);
    if (bDown != bPTTKeyDown)
    {
        bPTTKeyDown = bDown;
        SetPushToTalkActive(bDown);
    }
}

void UTeamSpeak_VoiceClientComponent::SetPTTKey(FKey NewKey)
{
    PTTKey = NewKey;
    // Drop any held state so a rebind mid-press doesn't leave the mic stuck open.
    bPTTKeyDown = false;
    if (bPTTEnabled)
    {
        SetPushToTalkActive(false);
    }
}

APlayerController* UTeamSpeak_VoiceClientComponent::GetOwningPC() const
{
    return Cast<APlayerController>(GetOwner());
}

void UTeamSpeak_VoiceClientComponent::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = GetOwningPC();
    if (!PC || !PC->IsLocalController())
    {
        return;
    }

    SubscribeToSdkStatus();
    EnsureMasterSynth();

    if (UTeamSpeak_SDKSubsystem* SDK = GetWorld()->GetGameInstance()->GetSubsystem<UTeamSpeak_SDKSubsystem>())
    {
        if (SDK->HasCarryOverHandler())
        {
            HandlerID = SDK->ConsumeCarryOverHandler();
            if (UTeamSpeak_AudioBridge* Bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge())
            {
                Bridge->SetActiveConnection(HandlerID, this);
            }
        }
    }

    if (bAutoConnect && HandlerID == 0)
    {
        Connect();
    }

    // Honor a config/default-enabled PTT: tick starts disabled, so enable it now if PTT is already on.
    RefreshTickEnabled();
}

void UTeamSpeak_VoiceClientComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnsubscribeFromSdkStatus();

    if (HandlerID != 0)
    {
        if (EndPlayReason == EEndPlayReason::LevelTransition && bCarryOverOnLevelTransition)
        {
            if (UWorld* W = GetWorld())
            {
                if (UGameInstance* GI = W->GetGameInstance())
                {
                    if (UTeamSpeak_SDKSubsystem* SDK = GI->GetSubsystem<UTeamSpeak_SDKSubsystem>())
                    {
                        SDK->StashCarryOverHandler(HandlerID);
                        HandlerID = 0;
                    }
                }
            }
        }

        if (HandlerID != 0)
        {
            // Synchronous teardown — Disconnect() now waits for the SDK's STATUS_DISCONNECTED
            // event before destroying the handler, but during component destruction we cannot
            // wait for that async callback. Send the disconnect command best-effort and
            // immediately tear down to avoid leaking the handler.
            int32 Error = 0;
            UTeamSpeakFunctionLibrary::TeamSpeak_stopConnection(HandlerID, QuitMessage, Error);
            TeardownHandler();
        }
    }

    Super::EndPlay(EndPlayReason);
}

void UTeamSpeak_VoiceClientComponent::Connect()
{
    if (HandlerID != 0)
    {
        return;
    }

    int32 Error = 0;
    HandlerID = UTeamSpeakFunctionLibrary::TeamSpeak_spawnNewServerConnectionHandler(0, Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok) || HandlerID == 0)
    {
        HandlerID = 0;
        return;
    }

    if (UTeamSpeak_AudioBridge* Bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge())
    {
        Bridge->SetActiveConnection(HandlerID, this);
    }

    SetMuteState(EAudioDirection::Input);
    SetMuteState(EAudioDirection::Output);

    UTeamSpeakFunctionLibrary::TeamSpeak_startConnection(
        HandlerID,
        Identity,
        Address,
        Port,
        Nickname,
        DefaultChannelPath,
        DefaultChannelPassword,
        ServerPassword,
        Error);

    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning,
            TEXT("startConnection failed on handler %d: error %d (address='%s' port=%d nickname='%s' identityEmpty=%d)"),
            HandlerID, Error, *Address, Port, *Nickname, Identity.IsEmpty() ? 1 : 0);

        if (UTeamSpeak_AudioBridge* Bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge())
        {
            Bridge->SetActiveConnection(0, nullptr);
        }
        UTeamSpeakFunctionLibrary::TeamSpeak_destroyServerConnectionHandler(HandlerID, Error);
        HandlerID = 0;
        LastStatus = EConnectStatus::STATUS_DISCONNECTED;
    }
}

void UTeamSpeak_VoiceClientComponent::ConnectWith(
    const FString& InIdentity,
    const FString& InNickname,
    const FString& InAddress,
    int32 InPort,
    const FString& InServerPassword,
    const TArray<FString>& InDefaultChannelPath,
    const FString& InDefaultChannelPassword)
{
    Identity = InIdentity;
    Nickname = InNickname;
    Address = InAddress;
    Port = InPort;
    ServerPassword = InServerPassword;
    DefaultChannelPath = InDefaultChannelPath;
    DefaultChannelPassword = InDefaultChannelPassword;
    Connect();
}

void UTeamSpeak_VoiceClientComponent::Disconnect()
{
    if (HandlerID == 0)
    {
        return;
    }

    int32 Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_stopConnection(HandlerID, QuitMessage, Error);
    // Don't destroy the handler here. The SDK fires STATUS_DISCONNECTED asynchronously after
    // the disconnect packet round-trips with the server; HandleSdkConnectStatusChanged picks
    // that event up, broadcasts to BP listeners, then calls TeardownHandler.
    // EndPlay path tears down synchronously when there's no time to wait.
}

void UTeamSpeak_VoiceClientComponent::TeardownHandler()
{
    if (HandlerID == 0)
    {
        return;
    }

    CloseCaptureDevice();
    ClosePlaybackDevice();

    if (UTeamSpeak_AudioBridge* Bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge())
    {
        Bridge->SetActiveConnection(0, nullptr);
    }

    const int32 OldHandlerID = HandlerID;
    HandlerID = 0;  // clear before destroy so any in-flight callbacks filter out via the handler-id guard
    int32 Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_destroyServerConnectionHandler(OldHandlerID, Error);
}

void UTeamSpeak_VoiceClientComponent::SubscribeToSdkStatus()
{
    if (bSubscribedToSdkStatus)
    {
        return;
    }
    if (UTeamSpeakFunctionLibrary* Lib = UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary())
    {
        Lib->onConnectStatusChangeEvent.AddDynamic(this, &UTeamSpeak_VoiceClientComponent::HandleSdkConnectStatusChanged);
        bSubscribedToSdkStatus = true;
    }
}

void UTeamSpeak_VoiceClientComponent::UnsubscribeFromSdkStatus()
{
    if (!bSubscribedToSdkStatus)
    {
        return;
    }
    if (UTeamSpeakFunctionLibrary* Lib = UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary())
    {
        Lib->onConnectStatusChangeEvent.RemoveDynamic(this, &UTeamSpeak_VoiceClientComponent::HandleSdkConnectStatusChanged);
    }
    bSubscribedToSdkStatus = false;
}

void UTeamSpeak_VoiceClientComponent::HandleSdkConnectStatusChanged(int32 InHandlerID, EConnectStatus NewStatus, int32 ErrorNumber)
{
    if (InHandlerID != HandlerID || HandlerID == 0)
    {
        return;
    }
    LastStatus = NewStatus;
    // Broadcast first so BP-side audio setup (e.g. master synth attach + Start) runs before
    // we open the capture device — preserves the original chain order.
    OnConnectStatusChanged.Broadcast(NewStatus, ErrorNumber);

    switch (NewStatus)
    {
    case EConnectStatus::STATUS_DISCONNECTED:
        // SDK has finished disconnecting (either user-initiated via Disconnect() above,
        // or server-initiated such as a kick or network drop). Tear down the handler.
        TeardownHandler();
        break;
    case EConnectStatus::STATUS_CONNECTED:
        // Playback before capture: OpenCaptureDevice's AEC-reference setup needs the
        // SDK's playback device to be associated with this handler first.
        OpenPlaybackDevice();
        OpenCaptureDevice();
        break;
    case EConnectStatus::STATUS_CONNECTION_ESTABLISHED:
        // Re-assert mute state in case it wasn't included in the initial handshake.
        SetMuteState(EAudioDirection::Input);
        SetMuteState(EAudioDirection::Output);
        break;
    default:
        break;
    }
}

void UTeamSpeak_VoiceClientComponent::OpenCaptureDevice()
{
    if (bCaptureOpen || HandlerID == 0)
    {
        return;
    }

    // Use the explicit UPROPERTY when set; otherwise resolve a usable native mode so we don't accidentally
    // open capture under the SDK's "custom" mode (which would be the default once the audio bridge has
    // registered the playback custom device).
    const FString EffectiveCaptureMode = CaptureDeviceMode.IsEmpty()
        ? ResolveCaptureDeviceMode()
        : CaptureDeviceMode;

    {
        int32 Error = 0;
        UTeamSpeakFunctionLibrary::TeamSpeak_openCaptureDevice(HandlerID, EffectiveCaptureMode, CaptureDeviceName, Error);
        if (Error == static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
        {
            bCaptureOpen = true;
        }
    }

    ApplyVADConfig();

    // Capture just (re)opened. If PTT is on, re-arm the gate so the next tick asserts mic-off for
    // this fresh capture — covers a reconnect where SetPTTEnabled isn't called again and the SDK's
    // CLIENT_INPUT_DEACTIVATED would otherwise default to "transmitting".
    if (bPTTEnabled)
    {
        bPTTKeyDown = false;
        bPTTGateApplied = false;
    }

    // Voice playback runs through a custom UE device, so the SDK's default mode-match AEC pairing
    // produces no reference. Point AEC at the OS render endpoint that ts3client's backend can loopback.
    //
    // Resolution policy:
    //   - If AECReferenceDeviceID is set, pass it through (with AECReferenceDeviceMode, or capture's
    //     mode if that is also empty).
    //   - If AECReferenceDeviceID is empty, query the SDK for its default playback device in the
    //     effective capture mode and use that device's ID. This is the "just works for 99% of users"
    //     path: SDK-default playback == OS default render endpoint, which UE is also rendering to.
    //
    // Future work: a UE-aware resolution would query FAudioDeviceManager for the actual output device
    // UE is using and translate its endpoint name to the SDK's device-ID format. Out of scope for v1
    // — most setups have UE on the OS default and this lookup matches what the user hears.
    //
    // Treat ERROR_not_implemented as non-fatal: it's returned by platforms whose backend cannot
    // loopback-capture an arbitrary render device (macOS today).
    FString AECMode   = AECReferenceDeviceMode;
    FString AECDevice = AECReferenceDeviceID;
    if (AECDevice.IsEmpty())
    {
        if (AECMode.IsEmpty())
        {
            AECMode = EffectiveCaptureMode;
        }
        if (AECMode.IsEmpty())
        {
            UE_LOG(LogTeamSpeak, Warning, TEXT("No effective capture mode available; skipping AEC reference setup"));
            return;
        }

        int32 LookupError = 0;
        const TArray<FString> Default = UTeamSpeakFunctionLibrary::TeamSpeak_getDefaultPlaybackDevice(AECMode, LookupError);
        // SDK returns [0]=display name, [1]=device ID; only the ID is meaningful to setAECReferenceDevice.
        if (LookupError != static_cast<int32>(ETeamSpeak_Error::ERROR_ok) || Default.Num() < 2)
        {
            UE_LOG(LogTeamSpeak, Warning, TEXT("getDefaultPlaybackDevice for mode '%s' failed (error %d, num=%d); skipping AEC reference setup"),
                   *AECMode, LookupError, Default.Num());
            return;
        }
        AECDevice = Default[1];
    }

    {
        int32 Error = 0;
        UTeamSpeakFunctionLibrary::TeamSpeak_setAECReferenceDevice(HandlerID, AECMode, AECDevice, Error);
        if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok)
            && Error != static_cast<int32>(ETeamSpeak_Error::ERROR_not_implemented))
        {
            UE_LOG(LogTeamSpeak, Warning, TEXT("setAECReferenceDevice(mode='%s', device='%s') failed with error %d on handler %d"),
                *AECMode, *AECDevice, Error, HandlerID);
        }
    }
}

void UTeamSpeak_VoiceClientComponent::CloseCaptureDevice()
{
    if (!bCaptureOpen || HandlerID == 0)
    {
        return;
    }
    int32 Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_closeCaptureDevice(HandlerID, Error);
    bCaptureOpen = false;
}

void UTeamSpeak_VoiceClientComponent::ReopenCaptureDevice()
{
    if (HandlerID == 0)
    {
        return;
    }
    CloseCaptureDevice();
    OpenCaptureDevice();
}

void UTeamSpeak_VoiceClientComponent::ApplyPlaybackDeviceReRegistration(TFunctionRef<void()> ReRegister)
{
    // Not connected: nothing is open on a handler, so just let the bridge swap the global
    // registration. The next OpenPlaybackDevice (on connect) uses the new rate.
    if (HandlerID == 0)
    {
        ReRegister();
        return;
    }

    const bool bWasPlaybackOpen = bPlaybackOpen;
    const bool bWasCaptureOpen  = bCaptureOpen;

    // Device must be closed before the bridge unregisters it.
    if (bWasPlaybackOpen)
    {
        ClosePlaybackDevice();
    }

    ReRegister();

    if (bWasPlaybackOpen)
    {
        OpenPlaybackDevice();
        // AEC pairing assumes the playback device is associated with the handler; closing and
        // reopening playback drops that association, so re-assert it by reopening capture.
        if (bWasCaptureOpen)
        {
            ReopenCaptureDevice();
        }
    }
}

void UTeamSpeak_VoiceClientComponent::OpenPlaybackDevice()
{
    if (bPlaybackOpen || HandlerID == 0)
    {
        return;
    }
    int32 Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_openPlaybackDevice(
        HandlerID, TEXT("custom"), TEXT("TeamSpeak_UE_Audio"), Error);
    if (Error == static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        bPlaybackOpen = true;
    }
    else
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("openPlaybackDevice failed on handler %d: error %d"), HandlerID, Error);
    }
}

void UTeamSpeak_VoiceClientComponent::ClosePlaybackDevice()
{
    if (!bPlaybackOpen || HandlerID == 0)
    {
        return;
    }
    int32 Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_closePlaybackDevice(HandlerID, Error);
    bPlaybackOpen = false;
}

void UTeamSpeak_VoiceClientComponent::EnsureMasterSynth()
{
    if (MasterSynth)
    {
        return;
    }
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }
    // AddComponentByClass mirrors what BP's "Add Component By Class" does: it attaches
    // the SceneComponent to the actor's RootComponent (or promotes it to root if none),
    // then finishes registration. NewObject+RegisterComponent skips that scene-tree step
    // and the audio thread never pulls samples from an orphaned SceneComponent.
    UActorComponent* Comp = Owner->AddComponentByClass(
        UTeamSpeak_SynthComponent::StaticClass(),
        /*bManualAttachment=*/ false,
        FTransform::Identity,
        /*bDeferredFinish=*/ false);
    MasterSynth = Cast<UTeamSpeak_SynthComponent>(Comp);
    if (MasterSynth)
    {
        MasterSynth->Start();
    }
}

void UTeamSpeak_VoiceClientComponent::SetMuteState(EAudioDirection Dir)
{
    auto bMute = Dir == EAudioDirection::Input ? bInputMuted : bOutputMuted;
    if (HandlerID == 0)
    {
        UE_LOG(LogTeamSpeak, Verbose, TEXT("Mute state (%s=%d) cached pre-connect; will apply when connection is established"),
                Dir == EAudioDirection::Input ? TEXT("input") : TEXT("output"), bMute ? 1 : 0);
        return;
    }

    int32 Error = 0;
    auto Property = Dir == EAudioDirection::Input ? ETeamSpeak_ClientProperties::CLIENT_INPUT_MUTED : ETeamSpeak_ClientProperties::CLIENT_OUTPUT_MUTED;
    UTeamSpeakFunctionLibrary::TeamSpeak_setClientSelfVariableAsint32(HandlerID, Property, bMute ? 1 : 0, Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("Failed to set %s mute state on handler %d: error %d"), Dir == EAudioDirection::Input ? TEXT("input") : TEXT("output"), HandlerID, Error);
        return;
    }
    UE_LOG(LogTeamSpeak, Log, TEXT("Set %s mute state to %d on handler %d"), Dir == EAudioDirection::Input ? TEXT("input") : TEXT("output"), bMute ? 1 : 0, HandlerID);

    auto ConnectionStatus = UTeamSpeakFunctionLibrary::TeamSpeak_getConnectionStatus(HandlerID, Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("Failed to get connection status for handler %d after setting mute state: error %d"), HandlerID, Error);
        return;
    }
    if (ConnectionStatus >= static_cast<int32>(EConnectStatus::STATUS_CONNECTED))
    {
        UTeamSpeakFunctionLibrary::TeamSpeak_flushClientSelfUpdates(HandlerID, FString(), Error);
        if (Error == static_cast<int32>(ETeamSpeak_Error::ERROR_ok_no_update))
        {
                UE_LOG(LogTeamSpeak, Verbose, TEXT("Flush dropped due to no change on handler %d"), HandlerID);
        }
        else if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
        {
            UE_LOG(LogTeamSpeak, Warning, TEXT("Failed to flush client self updates after changing mute state on handler %d: error %d"), HandlerID, Error);
        }
    }
}

void UTeamSpeak_VoiceClientComponent::SetInputMuted(bool bMuted)
{
    bInputMuted = bMuted;
    SetMuteState(EAudioDirection::Input);
}

void UTeamSpeak_VoiceClientComponent::SetOutputMuted(bool bMuted)
{
    bOutputMuted = bMuted;
    SetMuteState(EAudioDirection::Output);
}

/* static */
UTeamSpeak_VoiceClientComponent* UTeamSpeak_VoiceClientComponent::GetVoiceClientComponent(APlayerController* PlayerController)
{
    return PlayerController ? PlayerController->FindComponentByClass<UTeamSpeak_VoiceClientComponent>() : nullptr;
}

/* static */
const TCHAR* UTeamSpeak_VoiceClientComponent::VADModeToConfigValue(ETeamSpeak_VADMode Mode)
{
    // SDK expects a NUMERIC string for vad_mode (verified live: "0"/"1"/"2" accepted, the word
    // strings "hybrid"/"automatic"/"volume_gate" return ERROR_sound_internal_preprocessor 2305).
    switch (Mode)
    {
        case ETeamSpeak_VADMode::Automatic:  return TEXT("0");
        case ETeamSpeak_VADMode::Hybrid:     return TEXT("1");
        case ETeamSpeak_VADMode::VolumeGate: return TEXT("2");
        default:                             return TEXT("0");
    }
}

void UTeamSpeak_VoiceClientComponent::ApplyVADConfig()
{
    // Preprocessor settings are bound to a capture device — only valid once one is open.
    if (HandlerID == 0 || !bCaptureOpen)
    {
        return;
    }

    int32 Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_setPreProcessorConfigValue(
        HandlerID, TEXT("vad"), bVADEnabled ? TEXT("true") : TEXT("false"), Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("setPreProcessorConfigValue(vad=%d) failed: error %d"), bVADEnabled ? 1 : 0, Error);
    }

    if (!bVADEnabled)
    {
        return;
    }

    Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_setPreProcessorConfigValue(
        HandlerID, TEXT("vad_mode"), VADModeToConfigValue(VADMode), Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("setPreProcessorConfigValue(vad_mode='%s') failed: error %d — verify the SDK mode string"),
            VADModeToConfigValue(VADMode), Error);
    }

    Error = 0;
    UTeamSpeakFunctionLibrary::TeamSpeak_setPreProcessorConfigValue(
        HandlerID, TEXT("voiceactivation_level"), *FString::SanitizeFloat(VADLevel), Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("setPreProcessorConfigValue(voiceactivation_level=%f) failed: error %d"), VADLevel, Error);
    }
}

void UTeamSpeak_VoiceClientComponent::SetVADEnabled(bool bEnabled)
{
    bVADEnabled = bEnabled;
    ApplyVADConfig();
}

void UTeamSpeak_VoiceClientComponent::SetVADMode(ETeamSpeak_VADMode Mode)
{
    VADMode = Mode;
    ApplyVADConfig();
}

void UTeamSpeak_VoiceClientComponent::SetVADLevel(float Level)
{
    // TS6-style threshold in dB: -50 (most sensitive) .. +50 (least). Maps to SDK voiceactivation_level.
    VADLevel = FMath::Clamp(Level, -50.f, 50.f);
    ApplyVADConfig();
}

void UTeamSpeak_VoiceClientComponent::SetVADModeByIndex(int32 ModeIndex)
{
    SetVADMode(static_cast<ETeamSpeak_VADMode>(FMath::Clamp(ModeIndex, 0, 2)));
}

void UTeamSpeak_VoiceClientComponent::SetPTTEnabled(bool bEnabled)
{
    bPTTEnabled = bEnabled;
    bPTTKeyDown = false;
    // Always clear the gate flag so the tick re-asserts mic-off for the current handler. Setting it
    // true here (the old behaviour) suppressed the tick's safety net, so a handler whose
    // CLIENT_INPUT_DEACTIVATED was stale — e.g. carried over across a PIE stop/play, or after a
    // reconnect — could be left transmitting until the first key press flipped it. With this false,
    // the next tick (HandlerID != 0) forces SetPushToTalkActive(false) regardless of the carried state.
    bPTTGateApplied = false;
    // PTT on -> start silent (the hotkey gates transmission); PTT off -> leave the mic open.
    SetPushToTalkActive(!bEnabled);
    RefreshTickEnabled();
}

void UTeamSpeak_VoiceClientComponent::RefreshTickEnabled()
{
    // PTT polling is the only per-frame work this component does. Tick stays off until PTT is on
    // (and stops again when it's off). While PTT is on but not yet connected the tick runs but
    // early-returns cheaply on HandlerID == 0 — no SDK calls — until the handler comes up, at which
    // point TickComponent asserts the initial mic-off gate.
    SetComponentTickEnabled(bPTTEnabled);
}

float UTeamSpeak_VoiceClientComponent::GetCurrentInputLevel(int32& Error) const
{
    Error = 0;
    if (HandlerID == 0 || !bCaptureOpen)
    {
        return 0.f;
    }
    return UTeamSpeakFunctionLibrary::TeamSpeak_getPreProcessorInfoValueFloat(HandlerID, TEXT("decibel_last_period"), Error);
}

bool UTeamSpeak_VoiceClientComponent::IsTalking() const
{
    if (HandlerID == 0)
    {
        return false;
    }
    int32 Error = 0;
    const int32 Flag = UTeamSpeakFunctionLibrary::TeamSpeak_getClientSelfVariableAsint32(
        HandlerID, ETeamSpeak_ClientProperties::CLIENT_FLAG_TALKING, Error);
    return Error == static_cast<int32>(ETeamSpeak_Error::ERROR_ok) && Flag != 0;
}

void UTeamSpeak_VoiceClientComponent::SetPushToTalkActive(bool bSpeaking)
{
    if (HandlerID == 0)
    {
        return;
    }

    int32 Error = 0;
    // CLIENT_INPUT_DEACTIVATED: 0 = transmission allowed, 1 = deactivated (local-only PTT gate).
    UTeamSpeakFunctionLibrary::TeamSpeak_setClientSelfVariableAsint32(
        HandlerID, ETeamSpeak_ClientProperties::CLIENT_INPUT_DEACTIVATED, bSpeaking ? 0 : 1, Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("PTT set CLIENT_INPUT_DEACTIVATED=%d failed on handler %d: error %d"), bSpeaking ? 0 : 1, HandlerID, Error);
        return;
    }

    UTeamSpeakFunctionLibrary::TeamSpeak_flushClientSelfUpdates(HandlerID, FString(), Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok)
        && Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok_no_update))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("PTT flushClientSelfUpdates failed on handler %d: error %d"), HandlerID, Error);
    }
}

void UTeamSpeak_VoiceClientComponent::SetPTTReleaseDelay(bool bActive, int32 DelayMs)
{
    // Global client config (not per-handler); no FunctionLibrary wrapper exists, call the SDK directly.
    const auto ActiveErr = ts3client_setGlobalConfigValue("input_deactivation_delay_active", bActive ? "1" : "0");
    if (ActiveErr != ERROR_ok)
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("setGlobalConfigValue(input_deactivation_delay_active=%d) failed: error %u"), bActive ? 1 : 0, ActiveErr);
    }

    const FString DelayStr = FString::FromInt(FMath::Max(0, DelayMs));
    const auto DelayErr = ts3client_setGlobalConfigValue("input_deactivation_delay_ms", TCHAR_TO_UTF8(*DelayStr));
    if (DelayErr != ERROR_ok)
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("setGlobalConfigValue(input_deactivation_delay_ms=%d) failed: error %u"), DelayMs, DelayErr);
    }
}

FString UTeamSpeak_VoiceClientComponent::ResolveCaptureDeviceMode() const
{
    int32 Error = 0;
    auto Mode = UTeamSpeakFunctionLibrary::TeamSpeak_getDefaultCaptureMode(Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("getDefaultCaptureMode failed with error %d"), Error);
        return {};
    }
    if (Mode != CustomModeId)
    {
        return Mode;
    }
    TArray<FString> CaptureModes = UTeamSpeakFunctionLibrary::TeamSpeak_getCaptureModeList(Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        UE_LOG(LogTeamSpeak, Warning, TEXT("getCaptureModeList failed with error %d"), Error);
        return {};
    }
    for (const FString& AvailableMode : CaptureModes)
    {
        if (AvailableMode != CustomModeId)
        {
            return AvailableMode;
        }
    }

    UE_LOG(LogTeamSpeak, Warning, TEXT("No non-custom capture mode available; capture cannot use the SDK's native backend"));
    return {};
}
