#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoreMinimal.h"

#include "TeamSpeak_Manager.h"
#include "audiobridge/TsSoundGeneratorMaster.h"
#include "audiobridge/TsSoundGeneratorClient.h"

#include "Templates/SharedPointer.h"
#include "Templates/UniquePtr.h"
#include "Containers/Set.h"

#include <map>

#include "TeamSpeak_AudioBridge.generated.h"

class UTeamSpeak_VoiceClientComponent;

UENUM(BlueprintType)
enum class ETeamSpeak_InputDeactivationStatusUpdate : uint8 {
    INPUT_ACTIVATED = 0,
    INPUT_DEACTIVATED = 1,
    INPUT_ACTIVATION_SUPPRESSED = 2,
    NO_UPDATE = 3
};

/**
 * 
 */
UCLASS()
class TEAMSPEAK_SDK_API UTeamSpeak_AudioBridge : public UBlueprintFunctionLibrary {
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Statics")
    static UTeamSpeak_AudioBridge* getUTeamSpeak_AudioBridge();

    // C++ only: lifecycle is owned by UTeamSpeak_SDKSubsystem (BP-visible).
    static void TeamSpeak_AudioBridge_initClientLib(int32& error);

    // C++ only: lifecycle is owned by UTeamSpeak_SDKSubsystem (BP-visible).
    static void TeamSpeak_AudioBridge_shutdown();

    // Published by the connection layer (UTeamSpeak_VoiceClientComponent) after spawning a handler.
    // AudioBridge callbacks filter on the ID; the component handle lets the bridge drive playback
    // re-open during a sample-rate re-registration. Pass (0, nullptr) to clear on disconnect.
    // ID and component always move together so they can never disagree.
    void SetActiveConnection(int32 InConnectionId, UTeamSpeak_VoiceClientComponent* InComponent)
    {
        TeamSpeakConnectionId = InConnectionId;
        ActivePlaybackComponent = InComponent;
    }

    // Ensure the custom playback device is registered at DesiredRate (the UE mixer rate reported
    // by the master generator). No-op if already registered at that rate. Game thread only;
    // closes playback, re-registers, reopens, flushes per-client buffers, and re-asserts AEC.
    void ReconcilePlaybackRate(int32 DesiredRate);

    FTeamSpeak_valueHandler<ETeamSpeak_InputDeactivationStatusUpdate>
    do_update_voice_targets(const TSet<int32>& client_ids, bool is_mic_activation_allowed);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Whisper", meta = (ToolTip = "Sets the whisperlist and enables the mic if the target array has been empty. Allow Input Activation can be wired with some PTT mechanism."))
    static ETeamSpeak_InputDeactivationStatusUpdate updateVoiceTargets(const TSet<int32>& client_ids, bool is_mic_activation_allowed, int32& error);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TeamSpeak)
    int32 TeamSpeakConnectionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TeamSpeak)
    TSet<int32> LastWhisperTargets;

	// store references to our synths
	TWeakPtr<TsSoundGeneratorMaster, ESPMode::ThreadSafe> m_master_sound_generator = nullptr;
	std::map<uint16, TWeakPtr<TsSoundGeneratorClient, ESPMode::ThreadSafe> > m_client_sound_generators;

private:
    // Request every live per-client generator drop audio buffered at the old device rate.
    void RequestClientBufferFlush();

    static UTeamSpeak_AudioBridge* utsAB;

    // The currently registered custom-device playback frequency. Tracks the master generator's
    // mixer rate; initialized to match the registration done in TeamSpeak_AudioBridge_initClientLib.
    int32 RegisteredPlayFrequency = 0;

    // Component owning the active handler's playback device; nullptr when disconnected.
    TWeakObjectPtr<UTeamSpeak_VoiceClientComponent> ActivePlaybackComponent;
};
