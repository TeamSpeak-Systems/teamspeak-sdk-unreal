#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"

#include "TeamSpeak_SynthComponent.generated.h"


UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class TEAMSPEAK_SDK_API UTeamSpeak_SynthComponent : public USynthComponent
{
	GENERATED_BODY()
	
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;
	virtual void BeginDestroy() override;

protected:
#if ENGINE_MAJOR_VERSION >= 5
	virtual ISoundGeneratorPtr CreateSoundGenerator(const FSoundGeneratorInitParams& InParams) override;
#else
	virtual ISoundGeneratorPtr CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels) override;
#endif
private:
	ISoundGeneratorPtr do_create_sound_generator(int32 InSampleRate, int32 InNumChannels);

	ISoundGeneratorPtr m_sound_generator;
};