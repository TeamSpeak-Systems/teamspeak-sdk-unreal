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

	virtual ISoundGeneratorPtr CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels) override;

private:

	ISoundGeneratorPtr m_sound_generator;
};