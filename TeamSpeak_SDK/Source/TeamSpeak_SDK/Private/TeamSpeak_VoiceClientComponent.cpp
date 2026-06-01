#include "TeamSpeak_VoiceClientComponent.h"

#include "TeamSpeak_SDKSubsystem.h"
#include "TeamSpeakFunctionLibrary.h"
#include "audiobridge/TeamSpeak_AudioBridge.h"
#include "audiobridge/TeamSpeak_SynthComponent.h"
#include <teamspeak/public_errors.h>

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// SDK's virtual capture/playback mode used for app-provided custom audio devices
// (the plugin's UTeamSpeak_AudioBridge registers under this mode). When custom is
// registered, the SDK may report it as the default capture mode — ResolveCaptureDeviceMode
// uses this constant to detect and skip past that case.
static constexpr const TCHAR* CustomModeId = TEXT("custom");

UTeamSpeak_VoiceClientComponent::UTeamSpeak_VoiceClientComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
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
            // Synchronous teardown — Disconnect() waits for the SDK's STATUS_DISCONNECTED
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

    {
        int32 Error = 0;
        UTeamSpeakFunctionLibrary::TeamSpeak_setPreProcessorConfigValue(
            HandlerID, TEXT("vad"), bVADEnabled ? TEXT("true") : TEXT("false"), Error);
        if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
        {
            UE_LOG(LogTeamSpeak, Warning, TEXT("setPreProcessorConfigValue(vad=%d) failed: error %d"),
                bVADEnabled ? 1 : 0, Error);
        }
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
