#include "TsSoundGeneratorMaster.h"

#include "TeamSpeak_Manager.h"
#include "TeamSpeakFunctionLibrary.h"
#include "teamspeak/clientlib.h"
#include "teamspeak/public_errors.h"

#include "Engine/Engine.h"

#include <limits>

static constexpr const bool kMuteMaster{ true };

TsSoundGeneratorMaster::TsSoundGeneratorMaster(uint64 connection_id, int32 in_samplerate, int32 in_channels)
	: m_connection_id(connection_id)
	, m_samplerate_in(in_samplerate)
	, m_channels_in(in_channels)
{
}

TsSoundGeneratorMaster::~TsSoundGeneratorMaster()
{
}

// UE wants us to return NumSamples when we have not written data for silence
// returning 0 apparently triggers OnEnd?
int32 TsSoundGeneratorMaster::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	static constexpr float kScaling = 1.f / static_cast<float>(std::numeric_limits<short>::max());
	if (NumSamples > m_samples.size())
		m_samples.resize(NumSamples);

	// we measure in frames instead of samples
	const auto error = ts3client_acquireCustomPlaybackData("TeamSpeak_UE_Audio", m_samples.data(), NumSamples / m_channels_in);
	if (!kMuteMaster)
	{
		switch (error)
		{
		case ERROR_ok:
		{
			// simply copy the buffer, converting to float
			for (auto i = decltype(NumSamples){0}; i < NumSamples; ++i)
			{
				const auto sample = static_cast<float>(m_samples[i]);
				OutAudio[i] = sample * kScaling;
			}
			break;
		}
		case ERROR_sound_no_data:
			// we didn't get data. could simply be that no one's talking
			// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Error acquiring playback data: No data"));
			break;
		case ERROR_sound_no_playback_device_available: // can happen due to threading?
			break;
		default:
			// an actual error
			// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Error acquiring playback data: %u"), error));
			break;
		}
	}
	return NumSamples;
}

int32 TsSoundGeneratorMaster::GetDesiredNumSamplesToRenderPerCallback() const
{
	return 1024;
}

void TsSoundGeneratorMaster::OnBeginGenerate()
{
	const auto error = ts3client_openPlaybackDevice(m_connection_id, "custom", "TeamSpeak_UE_Audio");
	if (error != 0)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Couldn't open custom device: %f"), error));
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("TeamSpeak: Custom device opened"));
	}
}

void TsSoundGeneratorMaster::OnEndGenerate()
{
	const auto error = ts3client_closePlaybackDevice(m_connection_id);
	if (error != 0)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Couldn't close custom device: %f"), error));
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TeamSpeak: Custom device closed"));
	}
}
