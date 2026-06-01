#pragma once

#include "Sound/SoundGenerator.h"

#include "DSP/Dsp.h"

#include <array>
#include <atomic>

/**
 * This Sound Generator routes a TeamSpeak client playback into UE
 */
class TEAMSPEAK_SDK_API TsSoundGeneratorClient : public ISoundGenerator
{
public:
	TsSoundGeneratorClient(int32 in_samplerate, int32 in_channels);
	~TsSoundGeneratorClient();

	// Called on the same thread as acquire playback data in master synth - UE audio OnGenerateAudio render thread
	// We therefor shouldn't need to worry about thread safety, unless UE uses several threads for synths
	void on_edit_playback_voice_data(int16* samples, int32 frames, int32 channels);

	// Called when a new buffer is required.
	int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override final;

	// Request that any audio still buffered at the previous device sample rate be discarded.
	// Set by the audio bridge during a custom-device re-registration (sample-rate change);
	// honored on the consumer (audio render) thread at the top of OnGenerateAudio so the
	// SPSC circular buffer is only ever drained from its reader thread.
	void RequestFlush() { m_flush_requested.store(true, std::memory_order_relaxed); }

	// Returns the number of samples to render per callback
	int32 GetDesiredNumSamplesToRenderPerCallback() const override final
	{
		return kDesiredNumSamplesToRenderPerCallback;
	}

	// Optional. Called on audio generator thread right when the generator begins generating.
	void OnBeginGenerate() override final;

	// Optional. Called on audio generator thread right when the generator ends generating.
	void OnEndGenerate() override final;

private:
	const int32 m_samplerate_in;
	const int32 m_channels_in;

	// TODO would be nice to get this programmatically from the audio mixer
	static constexpr int32 kDesiredNumSamplesToRenderPerCallback{ 1024 };
	static constexpr size_t kBuffersize{ kDesiredNumSamplesToRenderPerCallback * 2 };
	
	// temp container for conversion to float
	std::array<int16, kBuffersize> m_samples;

	Audio::TCircularAudioBuffer<int16> m_circular_buffer{ kBuffersize };
	int32 m_overflows{ 0 };

	// Set from the game thread (bridge), consumed/cleared on the audio render thread.
	std::atomic<bool> m_flush_requested{ false };
};
