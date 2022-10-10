#include "TeamSpeak_SynthComponent.h"

#include "audiobridge/TeamSpeak_AudioBridge.h"
#include "TsSoundGeneratorMaster.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

bool UTeamSpeak_SynthComponent::Init(int32& SampleRate)
{
	NumChannels = 2;
	SampleRate = 48000;
	return true;
}

void UTeamSpeak_SynthComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

ISoundGeneratorPtr UTeamSpeak_SynthComponent::do_create_sound_generator(int32 InSampleRate, int32 InNumChannels)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Master Bus: InSampleRate: %d InNumChannels %d"), InSampleRate, InNumChannels));
	auto* audio_bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge();
	if (audio_bridge)
	{
		auto shared = MakeShared<TsSoundGeneratorMaster, ESPMode::ThreadSafe>(audio_bridge->TeamSpeakConnectionId, InSampleRate, InNumChannels);
		m_sound_generator = shared;
		audio_bridge->m_master_sound_generator = shared;
	}
	return m_sound_generator;
}

#if ENGINE_MAJOR_VERSION >= 5
ISoundGeneratorPtr UTeamSpeak_SynthComponent::CreateSoundGenerator(const FSoundGeneratorInitParams& InParams)
{
	// TODO: InParams has also:
	// int32 NumFramesPerCallback
	// uint64 InstanceID
	// FString 	GraphName
	// bool bIsPreviewSound
	return do_create_sound_generator(InParams.SampleRate, InParams.NumChannels);
}
#else
ISoundGeneratorPtr UTeamSpeak_SynthComponent::CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels)
{
	return do_create_sound_generator(InSampleRate, InNumChannels);
}
#endif


