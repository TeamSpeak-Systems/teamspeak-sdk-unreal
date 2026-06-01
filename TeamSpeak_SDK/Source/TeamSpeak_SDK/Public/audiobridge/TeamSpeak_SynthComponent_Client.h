#pragma once

#include "TsSoundGeneratorClient.h"

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"

#include "TeamSpeak_SynthComponent_Client.generated.h"


UCLASS(ClassGroup = TeamSpeak, meta = (BlueprintSpawnableComponent))
class TEAMSPEAK_SDK_API UTeamSpeak_SynthComponent_Client : public USynthComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	void TeamSpeak_AssignSynthToClient(int32 serverConnectionHandlerID, int32 clientId);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	void TeamSpeak_DeassignSynthFromClient();

	void BeginDestroy() override final;
protected:
	// Called when synth is created
	bool Init(int32& SampleRate) override final;

#if ENGINE_MAJOR_VERSION >= 5
	ISoundGeneratorPtr CreateSoundGenerator(const FSoundGeneratorInitParams& InParams) override final;
#else
	ISoundGeneratorPtr CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels) override final;
#endif

private:
	ISoundGeneratorPtr do_create_sound_generator(int32 InSampleRate, int32 InNumChannels);

	void maybe_add_to_manager();
	void maybe_remove_from_manager();

	TSharedPtr<TsSoundGeneratorClient, ESPMode::ThreadSafe> m_sound_generator;

	int32 m_connection_id = 0;
	int32 m_client_id = 0;
};