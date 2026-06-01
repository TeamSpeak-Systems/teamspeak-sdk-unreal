#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/Function.h"
#include "TeamSpeakFunctionLibrary.h"
#include "TeamSpeak_VoiceClientComponent.generated.h"

class APlayerController;
class UTeamSpeak_SynthComponent;

// Fired when connect status for THIS component's handler changes. Filtered to our handler ID.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeak_OnVoiceConnectStatusChanged, EConnectStatus, NewStatus, int32, ErrorNumber);

/**
 * Owns the TeamSpeak SDK connection lifecycle for the local player.
 *
 * Attach to a PlayerController. Spatialization is handled separately by
 * UTeamSpeak_SynthComponent_Client instances attached to remote-player actors;
 * Unreal's audio engine drives listener position from its own audio listener,
 * so this component does not track pawns or push listener attributes.
 */
UCLASS(Blueprintable, ClassGroup = (TeamSpeak), Config = Game, meta = (BlueprintSpawnableComponent))
class TEAMSPEAK_SDK_API UTeamSpeak_VoiceClientComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTeamSpeak_VoiceClientComponent();

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

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION()
    void HandleSdkConnectStatusChanged(int32 InHandlerID, EConnectStatus NewStatus, int32 ErrorNumber);

private:
    APlayerController* GetOwningPC() const;
    void SubscribeToSdkStatus();
    void UnsubscribeFromSdkStatus();
    void OpenCaptureDevice();
    void CloseCaptureDevice();
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

    UPROPERTY(Transient)
    int32 HandlerID = 0;

    UPROPERTY(Transient)
    EConnectStatus LastStatus = EConnectStatus::STATUS_DISCONNECTED;

    bool bSubscribedToSdkStatus = false;
    bool bCaptureOpen = false;
    bool bPlaybackOpen = false;

    UPROPERTY(Transient)
    TObjectPtr<UTeamSpeak_SynthComponent> MasterSynth = nullptr;
};
