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

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
    static void TeamSpeak_AudioBridge_initClientLib(int32& error);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
    static void TeamSpeak_AudioBridge_shutdown();

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
    static UTeamSpeak_AudioBridge* utsAB;
};
