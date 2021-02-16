#include "audiobridge/TeamSpeak_AudioBridge.h"

#include "audiobridge/TeamSpeak_SynthComponent.h"

#include "TeamSpeak_Helpers.h"

#include "teamspeak/clientlib.h"
#include "teamlog/logtypes.h"
#include "TeamSpeak_Manager.h"
#include "TeamSpeakFunctionLibrary.h"

#include "Engine/Engine.h"

#include <cstring>

using connection_id_t = uint64;
using client_id_t = anyID;

using audio_buffer_t = short;

using namespace com::teamspeak::helpers;

UTeamSpeak_AudioBridge* UTeamSpeak_AudioBridge::utsAB = nullptr;

UTeamSpeak_AudioBridge::UTeamSpeak_AudioBridge(const class FObjectInitializer& PCIP)
    : Super(PCIP)
{
    UTeamSpeak_AudioBridge::utsAB = this;
}

UTeamSpeak_AudioBridge* UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge()
{
    return utsAB;
}

void UTeamSpeak_AudioBridge::TeamSpeak_AudioBridge_initClientLib(int32& error)
{
#if !UE_SERVER
    auto audio_funcs = Custom_Function_Function_Pointer();
    memset(&audio_funcs, 0, sizeof(Custom_Function_Function_Pointer));

    // playback chain insert 1
    audio_funcs.on_edit_playback_voice_data_ptr = [](
        connection_id_t connection_id, client_id_t client_id,
        audio_buffer_t* samples, int32 frames, int32 channels) -> void
    {
        auto* audio_bridge = utsAB;
        if (!audio_bridge) return;

        const auto it = audio_bridge->m_client_sound_generators.find(client_id);
        if (it != audio_bridge->m_client_sound_generators.end())
        {
            auto client_sound_generator_ptr = it->second.Pin();
            if (client_sound_generator_ptr.IsValid())
            {
                auto* client_sound_generator = client_sound_generator_ptr.Get();
                client_sound_generator->on_edit_playback_voice_data(samples, frames, channels);
                // silence client on master
                const auto sample_count = frames * channels;
                for (auto i = decltype(sample_count){0}; i < sample_count; ++i)
                {
                    samples[i] = 0;
                }
            }
        }

    };

    // playback chain insert 2
    audio_funcs.on_edit_post_process_voide_data_ptr = nullptr;

    // playback chain master insert
    audio_funcs.on_edit_mixed_playback_voide_data_ptr = nullptr; /*[](
        connection_id_t connection_id,
        audio_buffer_t* samples, int frames, int channels,
        const unsigned int* channel_speaker_array,
        unsigned int* channel_fill_mask) -> void
    {
    };*/

    // capture chain insert
    audio_funcs.on_edit_captured_voice_data_ptr = [](
        connection_id_t connection_id,
        audio_buffer_t* samples, int frames, int channels,
        int* edited) -> void
    {

    };

    // manipulate spatialization (incoming client audio)
    audio_funcs.on_custom_3d_rolloff_calc_client_ptr = nullptr; /*[](
        connection_id_t connection_id, client_id_t client_id,
        float distance, float* volume) -> void
    {

    };*/

    // manipulate spatialization (wave handle playback)
    audio_funcs.on_custom_3d_rollof_calc_wave_ptr = nullptr; /*[](
        connection_id_t connection_id, uint64 wave_handle,
        float distance, float* volume) -> void
    {

    };*/

    auto soundbackend_folder = UTeamSpeakFunctionLibrary::getSoundBackendDir_Editor();
    const auto resources_folder = fstring_to_utf8(*soundbackend_folder);

    auto* teamspeak_manager = TeamSpeak_Manager::get_instance();
    if (teamspeak_manager)
    {
        {
            constexpr const auto* logFileFolder = "";
            const auto result = teamspeak_manager->UE_TS3_SDK_initClientLib(LogTypes::LogType_USERLOGGING, logFileFolder, resources_folder.data(), &audio_funcs);
            if (result.errorCode)
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error initializing TeamSpeak client library: %f"), result.errorCode));
            }
            else
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TeamSpeak Client library: initialized"));
            }
        }

        {
            const auto result = teamspeak_manager->UE_TS3_SDK_spawnNewServerConnectionHandler(0);
            if (result.errorCode)
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error spawning server connection handler: %f"), result.errorCode));
            }
            else
            {
                auto* thiz = getUTeamSpeak_AudioBridge();
                if (thiz)
                    thiz->TeamSpeakConnectionId = result.value;
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TeamSpeak: Connection Handler Spawned"));
            }
        }

        {
            const auto result = teamspeak_manager->UE_TS3_SDK_registerCustomDevice(TEXT("TeamSpeak_UE_Audio"), TEXT("TeamSpeak_UE_Audio"), 48000, 1, 48000, 2);
            if (result.errorCode)
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Couldn't register custom device: %f"), result.errorCode));
            }
            else
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TeamSpeak: Custom device registered"));
            }
        }
    }
#endif
}

void UTeamSpeak_AudioBridge::TeamSpeak_AudioBridge_shutdown()
{
#if !UE_SERVER
    auto* teamspeak_manager = TeamSpeak_Manager::get_instance();
    auto* audio_bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge();
    if (teamspeak_manager && audio_bridge)
    {
        audio_bridge->m_master_sound_generator.Reset();
        audio_bridge->m_client_sound_generators.clear();
        teamspeak_manager->UE_TS3_SDK_unregisterCustomDevice(TEXT("TeamSpeak_UE_Audio"));
        teamspeak_manager->UE_TS3_SDK_destroyServerConnectionHandler(audio_bridge->TeamSpeakConnectionId);
        teamspeak_manager->UE_TS3_SDK_destroyClientLib();
        // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TeamSpeak: shutdown complete"));
    }
    else
    {
        // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TeamSpeak: shutdown failed"));
        ts3client_logMessage("TeamSpeak: shutdown failed", LogLevel_ERROR, "TeamSpeak AudioBridge", audio_bridge ? audio_bridge->TeamSpeakConnectionId : 0);
    }
#endif
}
