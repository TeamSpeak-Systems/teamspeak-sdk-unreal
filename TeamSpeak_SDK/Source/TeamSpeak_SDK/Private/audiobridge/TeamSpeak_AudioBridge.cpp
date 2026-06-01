#include "audiobridge/TeamSpeak_AudioBridge.h"

#include "audiobridge/TeamSpeak_SynthComponent.h"

#include "TeamSpeak_VoiceClientComponent.h"
#include "TeamSpeak_Helpers.h"

#include "teamspeak/clientlib.h"
#include "teamspeak/public_errors.h"
#include "teamlog/logtypes.h"
#include "TeamSpeakFunctionLibrary.h"

#include "Engine/Engine.h"

#include <cstring>
#include <limits>
#include <vector>

using connection_id_t = uint64;
using client_id_t = anyID;

using audio_buffer_t = short;

using namespace com::teamspeak::helpers;

UTeamSpeak_AudioBridge* UTeamSpeak_AudioBridge::utsAB = nullptr;

namespace
{
    // Custom device identity and fixed parameters. Only the playback frequency varies at runtime
    // (tracked to the UE mixer rate); the capture half is unused (mic opens in a native mode).
    const TCHAR* const kCustomDeviceId = TEXT("TeamSpeak_UE_Audio");
    constexpr int32 kCustomCaptureFrequency = 48000;
    constexpr int32 kCustomCaptureChannels  = 1;
    constexpr int32 kCustomPlaybackChannels = 2;
    // Best-effort initial registration rate; the first master generator reconciles to the real
    // mixer rate. 48000 is the common case, so a matching mixer means zero reconcile churn.
    constexpr int32 kDefaultPlaybackFrequency = 48000;
}

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

    audio_funcs.on_edit_captured_voice_data_preprocess_ptr = nullptr;

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

    audio_funcs.on_talk_status_changed_ptr = nullptr;

    auto soundbackend_folder = UTeamSpeakFunctionLibrary::getSoundBackendDir_Editor();
    const auto resources_folder = fstring_to_utf8(*soundbackend_folder);

    auto custom_client_move_common = [](uint64 connection_id, anyID client_id, uint64 old_channel_id, uint64 new_channel_id, Visibility visibility) {
        auto* audio_bridge = utsAB;
        if (!audio_bridge)
            return;
        if (audio_bridge->TeamSpeakConnectionId != connection_id)
            return;

        if (new_channel_id == 0)
            audio_bridge->m_client_sound_generators.erase(client_id);
    };

    auto* teamspeak_manager = TeamSpeak_Manager::get_instance();
    if (teamspeak_manager)
    {
        {
            constexpr const auto* logFileFolder = "";
            const auto result = teamspeak_manager->UE_TS3_SDK_initClientLib(LogTypes::LogType_USERLOGGING | LogTypes::LogType_FILE, logFileFolder, resources_folder.data(), &audio_funcs, custom_client_move_common);
            if (result.errorCode)
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error initializing TeamSpeak client library: %f"), result.errorCode));
            }
            else
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TeamSpeak Client library: initialized"));
            }
        }

        // Connection handler is spawned by the connection layer (UTeamSpeak_VoiceClientComponent).
        // It registers itself via SetActiveConnection() so AudioBridge callbacks filter on it and
        // the bridge can reopen its playback device during a sample-rate re-registration.

        {
            const auto result = teamspeak_manager->UE_TS3_SDK_registerCustomDevice(kCustomDeviceId, kCustomDeviceId, kCustomCaptureFrequency, kCustomCaptureChannels, kDefaultPlaybackFrequency, kCustomPlaybackChannels);
            if (result.errorCode)
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Couldn't register custom device: %f"), result.errorCode));
            }
            else
            {
                // Static method: seed the singleton's tracked rate to match what we just registered.
                if (UTeamSpeak_AudioBridge* bridge = getUTeamSpeak_AudioBridge())
                {
                    bridge->RegisteredPlayFrequency = kDefaultPlaybackFrequency;
                }
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
        teamspeak_manager->UE_TS3_SDK_unregisterCustomDevice(kCustomDeviceId);
        audio_bridge->RegisteredPlayFrequency = 0;
        // Connection handler is destroyed by the connection layer (UTeamSpeak_VoiceClientComponent::Disconnect).
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

void UTeamSpeak_AudioBridge::RequestClientBufferFlush()
{
    // Runs on the game thread, after playback has been closed (see ReconcilePlaybackRate), so the
    // SDK is not pushing into these buffers. Each generator drains itself on its own reader thread.
    for (auto& entry : m_client_sound_generators)
    {
        if (auto pinned = entry.second.Pin())
        {
            pinned->RequestFlush();
        }
    }
}

void UTeamSpeak_AudioBridge::ReconcilePlaybackRate(int32 DesiredRate)
{
#if !UE_SERVER
    check(IsInGameThread());

    if (DesiredRate <= 0 || DesiredRate == RegisteredPlayFrequency)
    {
        return;
    }

    auto* teamspeak_manager = TeamSpeak_Manager::get_instance();
    if (!teamspeak_manager)
    {
        return;
    }

    // The device must not be open while we unregister it, and stale-rate buffers must be dropped.
    // Done as a callback so the component keeps ownership of its bPlaybackOpen / AEC bookkeeping
    // and runs it in the closed window between its own playback close and re-open.
    auto ReRegister = [this, teamspeak_manager, DesiredRate]()
    {
        RequestClientBufferFlush();
        teamspeak_manager->UE_TS3_SDK_unregisterCustomDevice(kCustomDeviceId);
        teamspeak_manager->UE_TS3_SDK_registerCustomDevice(
            kCustomDeviceId, kCustomDeviceId,
            kCustomCaptureFrequency, kCustomCaptureChannels,
            DesiredRate, kCustomPlaybackChannels);
        RegisteredPlayFrequency = DesiredRate;
    };

    if (UTeamSpeak_VoiceClientComponent* Component = ActivePlaybackComponent.Get())
    {
        Component->ApplyPlaybackDeviceReRegistration(ReRegister);
    }
    else
    {
        // No connected handler: just swap the global registration. The next OpenPlaybackDevice
        // (on connect) opens against the corrected rate.
        ReRegister();
    }
#endif
}

FTeamSpeak_valueHandler<ETeamSpeak_InputDeactivationStatusUpdate> UTeamSpeak_AudioBridge::do_update_voice_targets(const TSet<int32>& client_ids, bool is_mic_activation_allowed)
{
#if !UE_SERVER
    auto input_deactivation_update = ETeamSpeak_InputDeactivationStatusUpdate::NO_UPDATE;
    auto need_action = LastWhisperTargets.Num() != client_ids.Num();
    if (!need_action)
    {
        auto union_set = client_ids.Union(LastWhisperTargets);
        need_action = union_set.Num() != client_ids.Num();
    }
    if (!need_action)
        return FTeamSpeak_valueHandler<ETeamSpeak_InputDeactivationStatusUpdate>(input_deactivation_update, ERROR_ok_no_update);

    // at this point we know last != incoming

    if (client_ids.Num() == 0)
    {
        // we stop transmission and return
        auto error = ts3client_setClientSelfVariableAsInt(TeamSpeakConnectionId, CLIENT_INPUT_DEACTIVATED, 1);
        input_deactivation_update = ETeamSpeak_InputDeactivationStatusUpdate::INPUT_DEACTIVATED;
        return FTeamSpeak_valueHandler<ETeamSpeak_InputDeactivationStatusUpdate>(input_deactivation_update, error);
    }

    if (LastWhisperTargets.Num() == 0 && is_mic_activation_allowed)
    {
        // we start transmission, return only on error
        auto error = ts3client_setClientSelfVariableAsInt(TeamSpeakConnectionId, CLIENT_INPUT_DEACTIVATED, 0);
        input_deactivation_update = ETeamSpeak_InputDeactivationStatusUpdate::INPUT_DEACTIVATED;
        if (error != ERROR_ok && error != ERROR_ok_no_update)
            return FTeamSpeak_valueHandler<ETeamSpeak_InputDeactivationStatusUpdate>(input_deactivation_update, error);
    }

    // last but not least, set the whisper list
    auto my_id = anyID{ 0 };
    /*auto error =*/ ts3client_getClientID(TeamSpeakConnectionId, &my_id);

    auto client_ids_vec = TArray<anyID>();
    for (auto client_id : client_ids.Array())
    {
        // filter own client_id, zeros, negatives and too large values just in case
        if (client_id > 0 && client_id < std::numeric_limits<anyID>::max() && client_id != my_id)
            client_ids_vec.Add(static_cast<anyID>(client_id));
    }
    if (client_ids_vec.Num() > 0) // given that, now add the zero terminator
        client_ids_vec.Add(0);

    // client id 0 means for ourselves. Technically it's possible to set other client's whisper lists with this.
    auto error = ts3client_requestClientSetWhisperList(TeamSpeakConnectionId, 0, nullptr, client_ids_vec.Num() > 0 ? client_ids_vec.GetData() : nullptr, nullptr);
    if (ERROR_ok == error)
        LastWhisperTargets = client_ids;

    return FTeamSpeak_valueHandler<ETeamSpeak_InputDeactivationStatusUpdate>(input_deactivation_update, error);
#endif
}

/* static */
ETeamSpeak_InputDeactivationStatusUpdate UTeamSpeak_AudioBridge::updateVoiceTargets(const TSet<int32>& client_ids, bool is_mic_activation_allowed, int32& error)
{
#if !UE_SERVER
    auto* audio_bridge = UTeamSpeak_AudioBridge::getUTeamSpeak_AudioBridge();
    if (audio_bridge)
    {
        auto result = audio_bridge->do_update_voice_targets(client_ids, is_mic_activation_allowed);
        error = result.getErrorCode();
        return result.getValue();
    }
    else
    {
        // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TeamSpeak: shutdown failed"));
        ts3client_logMessage("TeamSpeak: set Voice Targets failed", LogLevel_ERROR, "TeamSpeak AudioBridge", audio_bridge ? audio_bridge->TeamSpeakConnectionId : 0);
        error = ERROR_clientlibrary_not_initialised;
        return ETeamSpeak_InputDeactivationStatusUpdate::NO_UPDATE;
    }
#endif
}
