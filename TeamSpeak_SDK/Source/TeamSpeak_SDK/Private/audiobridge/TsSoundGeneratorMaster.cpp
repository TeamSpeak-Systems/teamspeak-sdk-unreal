#include "audiobridge/TsSoundGeneratorMaster.h"

#include "audiobridge/TeamSpeak_AudioBridge.h"

#include "teamspeak/clientlib.h"
#include "teamspeak/public_errors.h"

#include "Async/Async.h"

#include <limits>

TsSoundGeneratorMaster::TsSoundGeneratorMaster(int32 in_samplerate, int32 in_channels)
	: m_samplerate_in(in_samplerate)
	, m_channels_in(in_channels)
{
	// The mixer rate UE drives this generator at (in_samplerate) is the rate the SDK custom
	// playback device must be registered with.
	// Registration / device open are game-thread + SDK operations, so marshal
	// off this (audio render) thread. A device-rate change recreates the generator, which
	// re-fires this and re-reconciles.
	const int32 rate = in_samplerate;
	AsyncTask(ENamedThreads::GameThread, [rate]()
	{
		if (UTeamSpeak_AudioBridge* bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge())
		{
			bridge->ReconcilePlaybackRate(rate);
		}
	});
}

TsSoundGeneratorMaster::~TsSoundGeneratorMaster()
{
}

// Returning 0 may signal end-of-stream to UE; always return NumSamples and silence the buffer
// when no SDK samples are available so playback keeps running across disconnect/reconnect.
int32 TsSoundGeneratorMaster::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	static constexpr float kScaling = 1.f / static_cast<float>(std::numeric_limits<short>::max());
	if (NumSamples > static_cast<int32>(m_samples.size()))
		m_samples.resize(NumSamples);

	const auto error = ts3client_acquireCustomPlaybackData("TeamSpeak_UE_Audio", m_samples.data(), NumSamples / m_channels_in);
	if (error == ERROR_ok)
	{
		for (int32 i = 0; i < NumSamples; ++i)
			OutAudio[i] = static_cast<float>(m_samples[i]) * kScaling;
	}
	else
	{
		FMemory::Memzero(OutAudio, sizeof(float) * NumSamples);
	}
	return NumSamples;
}

int32 TsSoundGeneratorMaster::GetDesiredNumSamplesToRenderPerCallback() const
{
	return 1024;
}
