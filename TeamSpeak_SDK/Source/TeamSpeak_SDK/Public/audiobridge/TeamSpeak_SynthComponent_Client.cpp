#include "TeamSpeak_SynthComponent_Client.h"

#include "audiobridge/TeamSpeak_AudioBridge.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void UTeamSpeak_SynthComponent_Client::TeamSpeak_AssignSynthToClient(int32 serverConnectionHandlerID, int32 clientId)
{
	m_connection_id = serverConnectionHandlerID;
	m_client_id = clientId;
	maybe_add_to_manager();
}

void UTeamSpeak_SynthComponent_Client::TeamSpeak_DeassignSynthFromClient()
{
	maybe_remove_from_manager();
}

bool UTeamSpeak_SynthComponent_Client::Init(int32& SampleRate)
{
	// As far as I'm informed, UE can only do spatialization for mono sources at this time
	NumChannels = 1;
	SampleRate = 48000;
	return true;
}

ISoundGeneratorPtr UTeamSpeak_SynthComponent_Client::do_create_sound_generator(int32 InSampleRate, int32 InNumChannels)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Client Bus: InSampleRate: %d InNumChannels %d"), InSampleRate, InNumChannels));
	m_sound_generator = MakeShared<TsSoundGeneratorClient, ESPMode::ThreadSafe>(InSampleRate, InNumChannels);
	maybe_add_to_manager();
	return m_sound_generator;
}

#if ENGINE_MAJOR_VERSION >= 5
ISoundGeneratorPtr UTeamSpeak_SynthComponent_Client::CreateSoundGenerator(const FSoundGeneratorInitParams& InParams)
{
	// TODO: InParams has also:
	// int32 NumFramesPerCallback
	// uint64 InstanceID
	// FString 	GraphName
	// bool bIsPreviewSound
	return do_create_sound_generator(InParams.SampleRate, InParams.NumChannels);
}
#else
ISoundGeneratorPtr UTeamSpeak_SynthComponent_Client::CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels)
{
	return do_create_sound_generator(InSampleRate, InNumChannels);
}
#endif

void UTeamSpeak_SynthComponent_Client::maybe_add_to_manager()
{
	if (m_sound_generator.IsValid() && !!m_connection_id && !!m_client_id)
	{
		auto* audio_bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge();
		if (audio_bridge)
		{
			audio_bridge->m_client_sound_generators.emplace(m_client_id, m_sound_generator);
		}
	}
}

void UTeamSpeak_SynthComponent_Client::maybe_remove_from_manager()
{
	if (!!m_connection_id && !!m_client_id)
	{
		auto* audio_bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge();
		if (audio_bridge)
		{
			audio_bridge->m_client_sound_generators.erase(m_client_id);
		}
	}
}
