#pragma once

#include "TsSoundGeneratorClient.h"

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"

#include "TeamSpeak_SynthComponent_Client.generated.h"


UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class TEAMSPEAK_SDK_API UTeamSpeak_SynthComponent_Client : public USynthComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	void TeamSpeak_AssignSynthToClient(int32 serverConnectionHandlerID, int32 clientId);

protected:
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;

	virtual ISoundGeneratorPtr CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels) override;

private:
	void maybe_add_to_manager();

	TSharedPtr<TsSoundGeneratorClient, ESPMode::ThreadSafe> m_sound_generator;

	int32 m_connection_id;
	int32 m_client_id;
};