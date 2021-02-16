#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoreMinimal.h"

#include "audiobridge/TsSoundGeneratorMaster.h"
#include "audiobridge/TsSoundGeneratorClient.h"

#include "Templates/SharedPointer.h"
#include "Templates/UniquePtr.h"

#include <map>

#include "TeamSpeak_AudioBridge.generated.h"

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TeamSpeak)
    int32 TeamSpeakConnectionId;

	// store references to our synths
	TWeakPtr<TsSoundGeneratorMaster, ESPMode::ThreadSafe> m_master_sound_generator = nullptr;
	std::map<uint16, TWeakPtr<TsSoundGeneratorClient, ESPMode::ThreadSafe> > m_client_sound_generators;

private:
    static UTeamSpeak_AudioBridge* utsAB;
};
