#include "TsSoundGeneratorClient.h"

#include "Engine/Engine.h"

#include <limits>

TsSoundGeneratorClient::TsSoundGeneratorClient(int32 in_samplerate, int32 in_channels)
    : m_samplerate_in(in_samplerate)
    , m_channels_in(in_channels)
{
}

TsSoundGeneratorClient::~TsSoundGeneratorClient()
{
}

void TsSoundGeneratorClient::on_edit_playback_voice_data(int16* samples, int32 frames, int32 channels)
{
    const auto sample_count = frames * channels;
    const auto overflow = sample_count - m_circular_buffer.Push(samples, sample_count);
    if (overflow)
    {
        m_overflows++;
        // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Overflow: %d"), m_overflows));
    }
}

int32 TsSoundGeneratorClient::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
    const auto samples_available = m_circular_buffer.Num();
    if (samples_available != 0)
    {
        NumSamples = FMath::Min(samples_available, static_cast<uint32>(NumSamples));
        /*const auto samples_read =*/ m_circular_buffer.Pop(m_samples.data(), NumSamples);
        static constexpr float kScaling = 1.f / static_cast<float>(std::numeric_limits<short>::max());
        for (auto i = decltype(NumSamples){0}; i < NumSamples; ++i)
        {
            const auto sample = static_cast<float>(m_samples[i]);
            OutAudio[i] = sample * kScaling;
        }
    }
    return NumSamples;
}

void TsSoundGeneratorClient::OnBeginGenerate()
{
}

void TsSoundGeneratorClient::OnEndGenerate()
{
}
