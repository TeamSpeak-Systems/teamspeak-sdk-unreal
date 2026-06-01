#include "audiobridge/TsSoundGeneratorClient.h"

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
    // A pending custom-device sample-rate change invalidates anything still buffered at the
    // old rate. Drain it here on the reader thread (the only thread allowed to Pop this SPSC
    // buffer) so playback resumes cleanly from the first sample produced at the new rate.
    if (m_flush_requested.exchange(false, std::memory_order_relaxed))
    {
        while (m_circular_buffer.Num() > 0)
        {
            const auto popped = m_circular_buffer.Pop(m_samples.data(), static_cast<int32>(m_samples.size()));
            if (popped == 0)
            {
                break; // defensive: never spin if the buffer reports data it won't yield
            }
        }
        m_overflows = 0;
    }

    const auto samples_read = m_circular_buffer.Pop(m_samples.data(), NumSamples);
    static constexpr float kScaling = 1.f / static_cast<float>(std::numeric_limits<short>::max());
    for (auto i = decltype(samples_read){0}; i < samples_read; ++i)
    {
        auto sample = static_cast<float>(m_samples[i]);
        OutAudio[i] = sample * kScaling;
    }
    return samples_read;
}

void TsSoundGeneratorClient::OnBeginGenerate()
{
}

void TsSoundGeneratorClient::OnEndGenerate()
{
}
