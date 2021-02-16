#pragma once

#include "Sound/SoundGenerator.h"

#include <vector>

/**
 * This Sound Generator routes the master bus of TeamSpeak playback into UE
 */
class TEAMSPEAK_SDK_API TsSoundGeneratorMaster : public ISoundGenerator
{
public:
	TsSoundGeneratorMaster(uint64 connection_id, int32 in_samplerate, int32 in_channels);
	~TsSoundGeneratorMaster();

	// Called when a new buffer is required.
	int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override final;

	// Returns the number of samples to render per callback
	int32 GetDesiredNumSamplesToRenderPerCallback() const override final;

	// Optional. Called on audio generator thread right when the generator begins generating.
	void OnBeginGenerate() override final;

	// Optional. Called on audio generator thread right when the generator ends generating.
	void OnEndGenerate() override final;

private:
	std::vector<int16> m_samples;
	int32 m_warmup{ 3 };
	int32 m_underruns{ 0 };

	const uint64 m_connection_id;
	const int32 m_samplerate_in;
	const int32 m_channels_in;
};
