#pragma once

#include "Sound/SoundGenerator.h"

#include <vector>

/**
 * This Sound Generator routes the master bus of TeamSpeak playback into UE
 */
class TEAMSPEAK_SDK_API TsSoundGeneratorMaster : public ISoundGenerator
{
public:
	TsSoundGeneratorMaster(int32 in_samplerate, int32 in_channels);
	~TsSoundGeneratorMaster();

	int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override final;
	int32 GetDesiredNumSamplesToRenderPerCallback() const override final;

private:
	std::vector<int16> m_samples;
	const int32 m_samplerate_in;
	const int32 m_channels_in;
};
