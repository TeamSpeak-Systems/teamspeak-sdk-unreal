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
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	void TeamSpeak_DeassignSynthFromClient();

protected:
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;

#if ENGINE_MAJOR_VERSION >= 5
	virtual ISoundGeneratorPtr CreateSoundGenerator(const FSoundGeneratorInitParams& InParams) override;
#else
	virtual ISoundGeneratorPtr CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels) override;
#endif

private:
	ISoundGeneratorPtr do_create_sound_generator(int32 InSampleRate, int32 InNumChannels);

	void maybe_add_to_manager();
	void maybe_remove_from_manager();

	TSharedPtr<TsSoundGeneratorClient, ESPMode::ThreadSafe> m_sound_generator;

	int32 m_connection_id = 0;
	int32 m_client_id = 0;
};