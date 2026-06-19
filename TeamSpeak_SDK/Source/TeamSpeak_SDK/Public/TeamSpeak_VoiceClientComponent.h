#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputCoreTypes.h"
#include "Templates/Function.h"
#include "TeamSpeakFunctionLibrary.h"
#include "TeamSpeak_VoiceClientComponent.generated.h"

class APlayerController;
class UTeamSpeak_SynthComponent;

// Fired when connect status for THIS component's handler changes. Filtered to our handler ID.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeak_OnVoiceConnectStatusChanged, EConnectStatus, NewStatus, int32, ErrorNumber);

// Voice Activity Detection mode, mirrors the TS6 client's VAD modes. The enum values intentionally
// match the SDK's numeric "vad_mode" preprocessor value (0=Automatic, 1=Hybrid, 2=VolumeGate), so
// the enum index doubles as the SDK config value (see VADModeToConfigValue).
UENUM(BlueprintType, meta = (DisplayName = "TeamSpeak VAD Mode"))
enum class ETeamSpeak_VADMode : uint8
{
    Automatic  UMETA(DisplayName = "Automatic"),
    Hybrid     UMETA(DisplayName = "Hybrid"),
    VolumeGate UMETA(DisplayName = "Volume Gate")
};

/**
 * Owns the TeamSpeak SDK connection lifecycle for the local player.
 *
 * Attach to a PlayerController. Spatialization is handled separately by
 * UTeamSpeak_SynthComponent_Client instances attached to remote-player actors;
 * Unreal's audio engine drives listener position from its own audio listener,
 * so this component does not track pawns or push listener attributes.
 */
UCLASS(Blueprintable, ClassGroup = (TeamSpeak), Config = Game, meta = (BlueprintSpawnableComponent, DisplayName = "TeamSpeak Voice Client Component"))
class TEAMSPEAK_SDK_API UTeamSpeak_VoiceClientComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTeamSpeak_VoiceClientComponent();

    // Convenience: returns the TeamSpeak voice component attached to the given PlayerController
    // (or null). Avoids GetComponentByClass + Cast boilerplate in Blueprints.
    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Voice")
    static UTeamSpeak_VoiceClientComponent* GetVoiceClientComponent(APlayerController* PlayerController);

    // Connect using the params currently set on this component. No-op if already connected.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Connection")
    void Connect();

    // Convenience: assign params then connect in one call.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Connection")
    void ConnectWith(
        const FString& InIdentity,
        const FString& InNickname,
        const FString& InAddress,
        int32 InPort,
        const FString& InServerPassword,
        const TArray<FString>& InDefaultChannelPath,
        const FString& InDefaultChannelPassword);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Connection")
    void Disconnect();

    // True only when the SDK has reported STATUS_CONNECTION_ESTABLISHED for our handler.
    // Spawning a handler / calling startConnection alone does NOT make this true.
    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Connection")
    bool IsConnected() const { return LastStatus == EConnectStatus::STATUS_CONNECTION_ESTABLISHED; }

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Connection")
    EConnectStatus GetConnectStatus() const { return LastStatus; }

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Connection")
    int32 GetHandlerID() const { return HandlerID; }

    // Bind in BP to react to connect/disconnect/timeout. Filtered to this component's handler.
    UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Connection")
    FTeamSpeak_OnVoiceConnectStatusChanged OnConnectStatusChanged;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    bool bAutoConnect = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TeamSpeak|Voice")
    bool bInputMuted = false;

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetInputMuted(bool bMuted);

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Voice")
    bool IsInputMuted() const { return bInputMuted; }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TeamSpeak|Voice")
    bool bOutputMuted = false;

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetOutputMuted(bool bMuted);

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Voice")
    bool IsOutputMuted() const { return bOutputMuted; }

    // Close and re-open the capture device, picking up any changes to CaptureDeviceMode /
    // CaptureDeviceName / AECReferenceDevice* UPROPERTYs. No-op if not connected.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void ReopenCaptureDevice();

    // Called by the audio bridge to swap the custom playback device's sample-rate registration.
    // Closes playback (if open), runs ReRegister in that closed window, reopens playback, and
    // re-asserts the AEC reference. Safe when not connected (just runs ReRegister). Game thread.
    void ApplyPlaybackDeviceReRegistration(TFunctionRef<void()> ReRegister);

    // If true and EndPlay reason is LevelTransition, the handler is stashed on
    // the subsystem instead of being torn down. The next level's component
    // adopts it on BeginPlay (no reconnect).
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    bool bCarryOverOnLevelTransition = false;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    FString Identity;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    FString Nickname;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    FString Address;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    int32 Port = 9987;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    FString ServerPassword;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    FString DefaultChannelPassword;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    TArray<FString> DefaultChannelPath;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Connection")
    FString QuitMessage = TEXT("leaving");

    // Capture device mode/name passed to openCaptureDevice. Empty = default.
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    FString CaptureDeviceMode;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    FString CaptureDeviceName;

    // Render device whose output is used as the AEC reference. Voice playback in this plugin runs through
    // a custom UE device, so the SDK's normal mode-match AEC pairing produces no reference. Pointing this
    // at the user's actual OS render endpoint lets the backend loopback what UE is rendering and AEC works
    // against the real echo path. Wired up at OpenCaptureDevice; on macOS the SDK call returns
    // ERROR_not_implemented and the plugin treats it as non-fatal.
    //
    // Leave empty for the common case: the plugin will resolve to the SDK's default playback device in
    // the active capture mode, which matches the OS default render endpoint for typical setups.
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    FString AECReferenceDeviceID;

    // Backend mode of the AEC reference device. Empty = use the capture device's mode (recommended).
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    FString AECReferenceDeviceMode;

    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    bool bVADEnabled = true;

    // VAD mode applied when VAD is enabled. Re-applied on every OpenCaptureDevice.
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    ETeamSpeak_VADMode VADMode = ETeamSpeak_VADMode::Automatic;

    // VAD activation threshold in dB (-50 most sensitive .. +50 least), mapped to SDK "voiceactivation_level".
    // Transmission starts when the live input dB rises above this threshold.
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice", meta = (ClampMin = "-50.0", ClampMax = "50.0"))
    float VADLevel = -30.f;

    // Enable/disable VAD ("vad" preprocessor). Applies immediately if a capture device is open.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetVADEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetVADMode(ETeamSpeak_VADMode Mode);

    // UI convenience: set VAD mode by ComboBox index (0=Automatic, 1=Hybrid, 2=VolumeGate). Clamped.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetVADModeByIndex(int32 ModeIndex);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetVADLevel(float Level);

    // Live input level (raw dB of the last processed period; typically -60..0) for a VAD meter
    // readout. Returns 0 if no capture device is open. Error receives the SDK error code.
    // Pull-based: call it when you actually draw a meter (e.g. a visible settings widget's Tick).
    // This component does no per-frame metering of its own; smoothing/curve/range are presentation
    // choices left to the consumer — e.g. Level01 = Clamp((Db + 60) / 60, 0, 1) then FInterpTo.
    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Voice")
    float GetCurrentInputLevel(int32& Error) const;

    // True while the local client is actively transmitting (SDK CLIENT_FLAG_TALKING). Drives a
    // TS6-style talking indicator. False when gated by PTT / below VAD threshold / not connected.
    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Voice")
    bool IsTalking() const;

    // Push-to-Talk primitive. Toggles CLIENT_INPUT_DEACTIVATED (local-only, not propagated as a
    // mute status). bSpeaking=true allows transmission (DEACTIVATED=0); false stops it (DEACTIVATED=1).
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetPushToTalkActive(bool bSpeaking);

    // Continue transmitting for DelayMs after PTT release so word-endings are not clipped
    // (SDK global config input_deactivation_delay_active / input_deactivation_delay_ms).
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetPTTReleaseDelay(bool bActive, int32 DelayMs);

    // Whether Push-to-Talk mode is active. When enabled, transmission starts deactivated and the
    // PTT hotkey gates it; when disabled, the mic is left open. SetPTTEnabled applies the matching
    // initial transmit state. The PTT hotkey handler should only act while IsPTTEnabled() is true.
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "TeamSpeak|Voice")
    bool bPTTEnabled = false;

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetPTTEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Voice")
    bool IsPTTEnabled() const { return bPTTEnabled; }

    // Key that gates Push-to-Talk transmission while held. Polled every tick on the owning
    // PlayerController (UPlayerInput state), bypassing Enhanced Input action bindings which are
    // fragile to register reliably in this minimal sample. Rebindable at runtime via SetPTTKey.
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Voice")
    FKey PTTKey = EKeys::V;

    // Live-rebind the PTT hotkey (e.g. from a "Set Hotkey" UI button). Resets transmit gate.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Voice")
    void SetPTTKey(FKey NewKey);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    void HandleSdkConnectStatusChanged(int32 InHandlerID, EConnectStatus NewStatus, int32 ErrorNumber);

private:
    APlayerController* GetOwningPC() const;
    void SubscribeToSdkStatus();
    void UnsubscribeFromSdkStatus();
    void OpenCaptureDevice();
    void CloseCaptureDevice();
    // Pushes bVADEnabled / VADMode / VADLevel to the SDK preprocessor. No-op if no handler/capture.
    void ApplyVADConfig();
    // Maps ETeamSpeak_VADMode to the SDK "vad_mode" config string (numeric: "0"/"1"/"2",
    // verified against SDK 3.5.0 — the word strings return ERROR_sound_internal_preprocessor).
    static const TCHAR* VADModeToConfigValue(ETeamSpeak_VADMode Mode);
    void OpenPlaybackDevice();
    void ClosePlaybackDevice();
    void EnsureMasterSynth();
    // Synchronous handler teardown — destroys the SDK handler and resets local state.
    // Disconnect()'s normal flow lets the SDK fire STATUS_DISCONNECTED first and teardown
    // happens in HandleSdkConnectStatusChanged. EndPlay calls this directly because we
    // can't wait for the async callback during component destruction.
    void TeardownHandler();
    /**
     * Returns the capture backend mode to open the SDK's native capture device under, skipping
     * past "custom" if that has been registered (the plugin's audio bridge does so, which causes
     * the SDK to report "custom" as the default capture mode). Falls back to the first non-custom
     * entry in the SDK's capture mode list (e.g. "windowsaudiosession", "pulseaudio"). Returns an
     * empty FString when lookup fails or no native mode is available.
     */
    FString ResolveCaptureDeviceMode() const;

    enum class EAudioDirection { Input, Output };
    void SetMuteState(EAudioDirection Dir);

    // Tick exists solely to poll the PTT hotkey, so enable it only while PTT is on. A component
    // that never uses PTT never ticks; turning PTT off stops the tick again.
    void RefreshTickEnabled();

    UPROPERTY(Transient)
    int32 HandlerID = 0;

    UPROPERTY(Transient)
    EConnectStatus LastStatus = EConnectStatus::STATUS_DISCONNECTED;

    bool bSubscribedToSdkStatus = false;
    bool bCaptureOpen = false;
    bool bPlaybackOpen = false;
    // Edge-detect state for the polled PTT key (see TickComponent).
    bool bPTTKeyDown = false;
    // Whether the initial PTT gate (mic-off-until-key) has been asserted on the live handler.
    // Lets us apply the gate once the connection exists even if PTT was enabled before connecting.
    bool bPTTGateApplied = false;

    UPROPERTY(Transient)
    TObjectPtr<UTeamSpeak_SynthComponent> MasterSynth = nullptr;
};
