#include "TeamSpeak_Manager.h"
#include "TeamSpeak_SDK.h"
#include "TeamSpeakFunctionLibrary.h"

#include <teamspeak/clientlib.h>
#include <teamspeak/clientlib_publicdefinitions.h>
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_errors.h>

#include <codecvt>

FString utf8_to_fstring(const char* data) {
    if (!data)
        return FString();

    return UTF8_TO_TCHAR(data);
}

std::string fstring_to_utf8(const TCHAR* value) {
    if (!value)
        return std::string();

    return TCHAR_TO_UTF8(value);
}

#define TS_TCHAR_TO_UTF8(X) \
    fstring_to_utf8(X).data()

#define TS_UTF8_TO_TCHAR(X) \
    utf8_to_fstring(X)

TeamSpeak_Manager* TeamSpeak_Manager::get_instance() {
    return UTeamSpeakFunctionLibrary::get_teampspeak_manager_ptr();
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_initClientLib(int32 usedLogTypes, const FString& logFileFolder, const FString& resourcesFolder, const Custom_Function_Function_Pointer* const custom_function_ptr) {
	ClientUIFunctions funcs;
	memset(&funcs, 0, sizeof(ClientUIFunctions));
	funcs.onConnectStatusChangeEvent = TeamSpeak_Manager::onConnectStatusChangeEvent;
	funcs.onServerProtocolVersionEvent = TeamSpeak_Manager::onServerProtocolVersionEvent;
	funcs.onNewChannelEvent = TeamSpeak_Manager::onNewChannelEvent;
	funcs.onNewChannelCreatedEvent = TeamSpeak_Manager::onNewChannelCreatedEvent;
	funcs.onDelChannelEvent = TeamSpeak_Manager::onDelChannelEvent;
	funcs.onChannelMoveEvent = TeamSpeak_Manager::onChannelMoveEvent;
	funcs.onUpdateChannelEvent = TeamSpeak_Manager::onUpdateChannelEvent;
	funcs.onUpdateChannelEditedEvent = TeamSpeak_Manager::onUpdateChannelEditedEvent;
	funcs.onUpdateClientEvent = TeamSpeak_Manager::onUpdateClientEvent;
	funcs.onClientMoveEvent = TeamSpeak_Manager::onClientMoveEvent;
	funcs.onClientMoveSubscriptionEvent = TeamSpeak_Manager::onClientMoveSubscriptionEvent;
	funcs.onClientMoveTimeoutEvent = TeamSpeak_Manager::onClientMoveTimeoutEvent;
	funcs.onClientMoveMovedEvent = TeamSpeak_Manager::onClientMoveMovedEvent;
	funcs.onClientKickFromChannelEvent = TeamSpeak_Manager::onClientKickFromChannelEvent;
	funcs.onClientKickFromServerEvent = TeamSpeak_Manager::onClientKickFromServerEvent;
	funcs.onClientIDsEvent = TeamSpeak_Manager::onClientIDsEvent;
	funcs.onClientIDsFinishedEvent = TeamSpeak_Manager::onClientIDsFinishedEvent;
	funcs.onServerEditedEvent = TeamSpeak_Manager::onServerEditedEvent;
	funcs.onServerUpdatedEvent = TeamSpeak_Manager::onServerUpdatedEvent;
	funcs.onServerErrorEvent = TeamSpeak_Manager::onServerErrorEvent;
	funcs.onServerStopEvent = TeamSpeak_Manager::onServerStopEvent;
	funcs.onTextMessageEvent = TeamSpeak_Manager::onTextMessageEvent;
	funcs.onTalkStatusChangeEvent = TeamSpeak_Manager::onTalkStatusChangeEvent;
	funcs.onIgnoredWhisperEvent = TeamSpeak_Manager::onIgnoredWhisperEvent;
	funcs.onConnectionInfoEvent = TeamSpeak_Manager::onConnectionInfoEvent;
	funcs.onServerConnectionInfoEvent = TeamSpeak_Manager::onServerConnectionInfoEvent;
	funcs.onChannelSubscribeEvent = TeamSpeak_Manager::onChannelSubscribeEvent;
	funcs.onChannelSubscribeFinishedEvent = TeamSpeak_Manager::onChannelSubscribeFinishedEvent;
	funcs.onChannelUnsubscribeEvent = TeamSpeak_Manager::onChannelUnsubscribeEvent;
	funcs.onChannelUnsubscribeFinishedEvent = TeamSpeak_Manager::onChannelUnsubscribeFinishedEvent;
	funcs.onChannelDescriptionUpdateEvent = TeamSpeak_Manager::onChannelDescriptionUpdateEvent;
	funcs.onChannelPasswordChangedEvent = TeamSpeak_Manager::onChannelPasswordChangedEvent;
	funcs.onPlaybackShutdownCompleteEvent = TeamSpeak_Manager::onPlaybackShutdownCompleteEvent;
	funcs.onSoundDeviceListChangedEvent = TeamSpeak_Manager::onSoundDeviceListChangedEvent;
	funcs.onUserLoggingMessageEvent = TeamSpeak_Manager::onUserLoggingMessageEvent;
	funcs.onProvisioningSlotRequestResultEvent = TeamSpeak_Manager::onProvisioningSlotRequestResultEvent;
	funcs.onCheckServerUniqueIdentifierEvent = TeamSpeak_Manager::onCheckServerUniqueIdentifierEvent;

    if (custom_function_ptr)
    {
        funcs.onEditPlaybackVoiceDataEvent = custom_function_ptr->on_edit_playback_voice_data_ptr;
        funcs.onEditPostProcessVoiceDataEvent = custom_function_ptr->on_edit_post_process_voide_data_ptr;
        funcs.onEditMixedPlaybackVoiceDataEvent = custom_function_ptr->on_edit_mixed_playback_voide_data_ptr;
        funcs.onEditCapturedVoiceDataEvent = custom_function_ptr->on_edit_captured_voice_data_ptr;
        funcs.onCustom3dRolloffCalculationClientEvent = custom_function_ptr->on_custom_3d_rolloff_calc_client_ptr;
        funcs.onCustom3dRolloffCalculationWaveEvent = custom_function_ptr->on_custom_3d_rollof_calc_wave_ptr;
    }

    return UE_TS3_SDK_initClientLib(usedLogTypes, logFileFolder, resourcesFolder, funcs);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_initClientLib(int32 usedLogTypes, const FString& logFileFolder, const FString& resourcesFolder, const ClientUIFunctions& funcs) {
    unsigned int error;
    if ((error = ts3client_initClientLib(&funcs, NULL, usedLogTypes, TS_TCHAR_TO_UTF8(*logFileFolder), TS_TCHAR_TO_UTF8(*resourcesFolder))) != ERROR_ok) {
        return FTeamSpeak_valueHandler<void*>(NULL, error);
    }
    return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_destroyClientLib() {
	unsigned int error;
	if ((error = ts3client_destroyClientLib()) != ERROR_ok) {
		return FTeamSpeak_valueHandler<void*>(NULL, error);
	}
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getClientLibVersion() {
	unsigned int error;
	char* result;
	if ((error = ts3client_getClientLibVersion(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getClientLibVersionNumber() {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getClientLibVersionNumber(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_spawnNewServerConnectionHandler(int32 port) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_spawnNewServerConnectionHandler(port, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_destroyServerConnectionHandler(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_destroyServerConnectionHandler(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_createIdentity() {
	unsigned int error;
	char* result;
	if ((error = ts3client_createIdentity(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_identityStringToUniqueIdentifier(const FString& identityString) {
	unsigned int error;
	char* result;
	if ((error = ts3client_identityStringToUniqueIdentifier(TS_TCHAR_TO_UTF8(*identityString), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(result, error);
}

FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>> TeamSpeak_Manager::UE_TS3_SDK_getPlaybackDeviceList(const FString& modeID) {
	unsigned int error;
	char*** ret;
	if ((error = ts3client_getPlaybackDeviceList(TS_TCHAR_TO_UTF8(*modeID), &ret)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>>(TArray<FTeamSpeak_2dArray>(), error);
	}	
    auto result = converting_helper::conv_charppp_to_2d_array(ret);

	ts3client_freeMemory(ret);
	return FTeamSpeak_valueHandler<std::remove_cv<decltype(result)>::type>(result, error);
}

FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>> TeamSpeak_Manager::UE_TS3_SDK_getCaptureDeviceList(const FString& modeID) {
	unsigned int error;
	char*** ret;
	if ((error = ts3client_getCaptureDeviceList(TS_TCHAR_TO_UTF8(*modeID), &ret)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>>(TArray<FTeamSpeak_2dArray>(), error);
	}
    auto result = converting_helper::conv_charppp_to_2d_array(ret);

    ts3client_freeMemory(ret);
    return FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>>(result, error);
}

FTeamSpeak_valueHandler<TArray<FString>> TeamSpeak_Manager::UE_TS3_SDK_getPlaybackModeList() {
	unsigned int error;
	char** result;
	TArray<FString> list;
	if ((error = ts3client_getPlaybackModeList(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FString>>(TArray<FString>(), error);
	}
    auto&& array = converting_helper::conv_charpp_to_array_fstring(result);

    ts3client_freeMemory(result);
    return FTeamSpeak_valueHandler<std::remove_reference<decltype(array)>::type>(array, error);
}

FTeamSpeak_valueHandler<TArray<FString>> TeamSpeak_Manager::UE_TS3_SDK_getCaptureModeList() {
	unsigned int error;
	char** result;
	TArray<FString> list;
	if ((error = ts3client_getCaptureModeList(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FString>>(TArray<FString>(), error);
	}
    auto&& array = converting_helper::conv_charpp_to_array_fstring(result);

    ts3client_freeMemory(result);
    return FTeamSpeak_valueHandler<std::remove_reference<decltype(array)>::type>(array, error);
}

FTeamSpeak_valueHandler<TArray<FString>> TeamSpeak_Manager::UE_TS3_SDK_getDefaultPlaybackDevice(const FString& modeID) {
	unsigned int error;
	char** result;
	TArray<FString> list;
	if ((error = ts3client_getDefaultPlaybackDevice(TS_TCHAR_TO_UTF8(*modeID), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FString>>(TArray<FString>(), error);
	}
    auto&& array = converting_helper::conv_charpp_to_array_fstring(result);

    ts3client_freeMemory(result);
    return FTeamSpeak_valueHandler<std::remove_reference<decltype(array)>::type>(array, error);
}

FTeamSpeak_valueHandler<TArray<FString>> TeamSpeak_Manager::UE_TS3_SDK_getDefaultCaptureDevice(const FString& modeID) {
	unsigned int error;
	char** result;
	TArray<FString> list;
	if ((error = ts3client_getDefaultCaptureDevice(TS_TCHAR_TO_UTF8(*modeID), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FString>>(TArray<FString>(), error);
	}
    auto&& array = converting_helper::conv_charpp_to_array_fstring(result);

    ts3client_freeMemory(result);
    return FTeamSpeak_valueHandler<std::remove_reference<decltype(array)>::type>(array, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getDefaultPlayBackMode() {
	unsigned int error;
	char* result;
	if ((error = ts3client_getDefaultPlayBackMode(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getDefaultCaptureMode() {
	unsigned int error;
	char* result;
	if ((error = ts3client_getDefaultCaptureMode(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_openPlaybackDevice(uint64 serverConnectionHandlerID, const FString& modeID, const FString& playbackDevice) {
	unsigned int error;
	error = ts3client_openPlaybackDevice(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*modeID), TS_TCHAR_TO_UTF8(*playbackDevice));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_openCaptureDevice(uint64 serverConnectionHandlerID, const FString& modeID, const FString& captureDevice) {
	unsigned int error;
	error = ts3client_openCaptureDevice(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*modeID), TS_TCHAR_TO_UTF8(*captureDevice));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<TArray<FString>> TeamSpeak_Manager::UE_TS3_SDK_getCurrentPlaybackDeviceName(uint64 serverConnectionHandlerID) {
	unsigned int error;
	char* result;
	int isDefault;
	if ((error = ts3client_getCurrentPlaybackDeviceName(serverConnectionHandlerID, &result, &isDefault)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FString>>(TArray<FString>(), error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	TArray<FString> buffer;
	buffer.Insert(copy, 0);
	buffer.Insert(FString::FromInt(isDefault), 1);
	return FTeamSpeak_valueHandler<TArray<FString>>(buffer, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getCurrentPlayBackMode(uint64 serverConnectionHandlerID) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getCurrentPlayBackMode(serverConnectionHandlerID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(result, error);
}

FTeamSpeak_valueHandler<TArray<FString>> TeamSpeak_Manager::UE_TS3_SDK_getCurrentCaptureDeviceName(uint64 serverConnectionHandlerID) {
	unsigned int error;
	char* result;
	int isDefault;
	if ((error = ts3client_getCurrentCaptureDeviceName(serverConnectionHandlerID, &result, &isDefault)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<FString>>(TArray<FString>(), error);
	}
    FString copy(result);
	ts3client_freeMemory(result);
	TArray<FString> buffer;
	buffer.Insert(copy, 0);
	buffer.Insert(FString::FromInt(isDefault), 1);
	return FTeamSpeak_valueHandler<TArray<FString>>(buffer, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getCurrentCaptureMode(uint64 serverConnectionHandlerID) {
	unsigned int error = 0;
	char* result;
	if ((error = ts3client_getCurrentCaptureMode(serverConnectionHandlerID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(result, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_initiateGracefulPlaybackShutdown(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_initiateGracefulPlaybackShutdown(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_closePlaybackDevice(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_closePlaybackDevice(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_closeCaptureDevice(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_closeCaptureDevice(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_activateCaptureDevice(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_activateCaptureDevice(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_playWaveFile(uint64 serverConnectionHandlerID, const FString& path) {
	unsigned int error;
	error = ts3client_playWaveFile(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*path));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_playWaveFileHandle(uint64 serverConnectionHandlerID, const FString& path, int32 loop, uint64 waveHandle) {
	unsigned int error;
	error = ts3client_playWaveFileHandle(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*path), loop, &waveHandle);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_pauseWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle, int32 pause) {
	unsigned int error;
	error = ts3client_pauseWaveFileHandle(serverConnectionHandlerID, waveHandle, pause);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_closeWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle) {
	unsigned int error;
	error = ts3client_closeWaveFileHandle(serverConnectionHandlerID, waveHandle);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_registerCustomDevice(const FString& deviceID, const FString& deviceDisplayName, int32 capFrequency, int32 capChannels, int32 playFrequency, int32 playChannels) {
	unsigned int error;
	error = ts3client_registerCustomDevice(TS_TCHAR_TO_UTF8(*deviceID), TS_TCHAR_TO_UTF8(*deviceDisplayName), capFrequency, playChannels, playFrequency, playChannels);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_unregisterCustomDevice(const FString& deviceID) {
	unsigned int error;
	error = ts3client_unregisterCustomDevice(TS_TCHAR_TO_UTF8(*deviceID));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_processCustomCaptureData(const FString& deviceName, const TArray<int16>& buffer, int32 samples) {
	unsigned int error;
	error = ts3client_processCustomCaptureData(TS_TCHAR_TO_UTF8(*deviceName), buffer.GetData(), samples);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_acquireCustomPlaybackData(const FString& deviceName, TArray<int32>& buffer, int32 samples) {
	unsigned int error;
	error = ts3client_acquireCustomPlaybackData(TS_TCHAR_TO_UTF8(*deviceName), (int16*)buffer.GetData(), samples);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setLocalTestMode(uint64 serverConnectionHandlerID, int32 status) {
	unsigned int error;
	error = ts3client_setLocalTestMode(serverConnectionHandlerID, status);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_startVoiceRecording(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_startVoiceRecording(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_stopVoiceRecording(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_stopVoiceRecording(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_allowWhispersFrom(uint64 serverConnectionHandlerID, int32 clID) {
	unsigned int error;
	error = ts3client_allowWhispersFrom(serverConnectionHandlerID, clID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_removeFromAllowedWhispersFrom(uint64 serverConnectionHandlerID, int32 clID) {
	unsigned int error;
	error = ts3client_removeFromAllowedWhispersFrom(serverConnectionHandlerID, clID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

const TS3_VECTOR vectorConverter_FVector(FVector v) {
	TS3_VECTOR ret;
	ret.x = v.X;
	ret.y = v.Y;
	ret.z = v.Z;
	return ret;
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_systemset3DListenerAttributes(uint64 serverConnectionHandlerID, FVector position, FVector forward, FVector up) {
	unsigned int error;
	const TS3_VECTOR _position = vectorConverter_FVector(position);
	const TS3_VECTOR _forward = vectorConverter_FVector(forward);
	const TS3_VECTOR _up = vectorConverter_FVector(up);
	error = ts3client_systemset3DListenerAttributes(serverConnectionHandlerID, &_position, &_forward, &_up);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_set3DWaveAttributes(uint64 serverConnectionHandlerID, uint64 waveHandle, FVector position) {
	unsigned int error;
	const TS3_VECTOR _position = vectorConverter_FVector(position);
	error = ts3client_set3DWaveAttributes(serverConnectionHandlerID, waveHandle, &_position);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_systemset3DSettings(uint64 serverConnectionHandlerID, float distanceFactor, float rolloffScale) {
	unsigned int error;
	error = ts3client_systemset3DSettings(serverConnectionHandlerID, distanceFactor, rolloffScale);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_channelset3DAttributes(uint64 serverConnectionHandlerID, int32 clientID, FVector position) {
	unsigned int error;
	const TS3_VECTOR _position = vectorConverter_FVector(position);
	error = ts3client_channelset3DAttributes(serverConnectionHandlerID, clientID, &_position);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<float> TeamSpeak_Manager::UE_TS3_SDK_getPreProcessorInfoValueFloat(uint64 serverConnectionHandlerID, const FString& ident) {
	unsigned int error;
	float result;
	if ((error = ts3client_getPreProcessorInfoValueFloat(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*ident), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<float>(.0f, error);
	}
	return FTeamSpeak_valueHandler<float>(result, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getPreProcessorConfigValue(uint64 serverConnectionHandlerID, const FString& ident) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getPreProcessorConfigValue(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*ident), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setPreProcessorConfigValue(uint64 serverConnectionHandlerID, const FString& ident, const FString& value) {
	unsigned int error;
	error = ts3client_setPreProcessorConfigValue(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*ident), TS_TCHAR_TO_UTF8(*value));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getEncodeConfigValue(uint64 serverConnectionHandlerID, const FString& ident) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getEncodeConfigValue(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*ident), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<float> TeamSpeak_Manager::UE_TS3_SDK_getPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const FString& ident) {
	unsigned int error;
	float result;
	if ((error = ts3client_getPlaybackConfigValueAsFloat(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*ident), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<float>(.0f, error);
	}
	return FTeamSpeak_valueHandler<float>(result, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setPlaybackConfigValue(uint64 serverConnectionHandlerID, const FString& ident, const FString& value) {
	unsigned int error;
	error = ts3client_setPlaybackConfigValue(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*ident), TS_TCHAR_TO_UTF8(*value));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setClientVolumeModifier(uint64 serverConnectionHandlerID, int32 clientID, float value) {
	unsigned int error;
	error = ts3client_setClientVolumeModifier(serverConnectionHandlerID, clientID, value);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_logMessage(const FString& logMessage, int32 severity, const FString& channel, uint64 logID){
	unsigned int error;
	error = ts3client_logMessage(TS_TCHAR_TO_UTF8(*logMessage), static_cast<enum LogLevel>(severity), TS_TCHAR_TO_UTF8(*channel), logID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setLogVerbosity(int32 logVerbosity) {
	unsigned int error;
	error = ts3client_setLogVerbosity(static_cast<enum LogLevel>(logVerbosity));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}
FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getErrorMessage(int32 errorCode) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getErrorMessage(errorCode, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_startConnection(uint64 serverConnectionHandlerID, const FString& identity, const FString& ip, int32 port, const FString& nickname, const TArray<FString>& defaultChannelArray, const FString& defaultChannelPassword, const FString& serverPassword) {
	unsigned int error;

    char** wr_channel_path;
    const size_t len = defaultChannelArray.Num() + 1;
    wr_channel_path = new char*[len];
    size_t i = 0;
    for (auto&& ch : defaultChannelArray)
    {
        size_t ch_len = ch.Len() + 1;
        wr_channel_path[i] = new char[ch_len];
        memset(wr_channel_path[i], 0, ch_len);
#ifdef _WIN32
        strcpy_s(wr_channel_path[i], ch_len, TS_TCHAR_TO_UTF8(*ch));
#else
        strcpy(wr_channel_path[i], TS_TCHAR_TO_UTF8(*ch));
#endif
        ++i;
    }
    wr_channel_path[i] = new char[1];
#ifdef _WIN32
    strcpy_s(wr_channel_path[i], 1, "");
#else
    strcpy(wr_channel_path[i], "");
#endif

	error = ts3client_startConnection(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*identity), TS_TCHAR_TO_UTF8(*ip), port, TS_TCHAR_TO_UTF8(*nickname), (const char**)wr_channel_path, TS_TCHAR_TO_UTF8(*defaultChannelPassword), TS_TCHAR_TO_UTF8(*serverPassword));

    for (i = 0; i < len; ++i)
    {
        delete wr_channel_path[i];
    }

    delete[] wr_channel_path;

	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_startConnectionWithChannelID(uint64 serverConnectionHandlerID, const FString& identity, const FString& ip, unsigned int port, const FString& nickname, uint64 defaultChannelId, const FString& defaultChannelPassword, const FString& serverPassword)
{
    unsigned int error;
    error = ts3client_startConnectionWithChannelID(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*identity), TS_TCHAR_TO_UTF8(*ip), port, TS_TCHAR_TO_UTF8(*nickname), defaultChannelId, TS_TCHAR_TO_UTF8(*defaultChannelPassword), TS_TCHAR_TO_UTF8(*serverPassword));
    return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_stopConnection(uint64 serverConnectionHandlerID, const FString& quitMessage) {
	unsigned int error;
	error = ts3client_stopConnection(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*quitMessage));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestClientMove(uint64 serverConnectionHandlerID, int32 clientID, uint64 newChannelID, const FString& password, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestClientMove(serverConnectionHandlerID, clientID, newChannelID, TS_TCHAR_TO_UTF8(*password), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestClientVariables(uint64 serverConnectionHandlerID, int32 clientID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestClientVariables(serverConnectionHandlerID, clientID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestClientKickFromChannel(uint64 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestClientKickFromChannel(serverConnectionHandlerID, clientID, TS_TCHAR_TO_UTF8(*kickReason), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestClientKickFromServer(uint64 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestClientKickFromServer(serverConnectionHandlerID, clientID, TS_TCHAR_TO_UTF8(*kickReason), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelDelete(uint64 serverConnectionHandlerID, uint64 channelID, int32 force, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelDelete(serverConnectionHandlerID, channelID, force, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelMove(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, uint64 newChannelOrder, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelMove(serverConnectionHandlerID, channelID, newChannelParentID, newChannelOrder, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestSendPrivateTextMsg(uint64 serverConnectionHandlerID, const FString& message, int32 targetClientID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestSendPrivateTextMsg(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*message), targetClientID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestSendChannelTextMsg(uint64 serverConnectionHandlerID, const FString& message, uint64 targetChannelID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestSendChannelTextMsg(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*message), targetChannelID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestSendServerTextMsg(uint64 serverConnectionHandlerID, const FString& message, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestSendServerTextMsg(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*message), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestConnectionInfo(uint64 serverConnectionHandlerID, int32 clientID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestConnectionInfo(serverConnectionHandlerID, clientID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestClientSetWhisperList(uint64 serverConnectionHandlerID, int32 clientID, const TArray<uint64>& targetChannelIDArray, const TArray<int32>& targetClientIDArray, const FString& returnCode) {
	unsigned int error;

	error = ts3client_requestClientSetWhisperList(serverConnectionHandlerID, clientID, targetChannelIDArray.GetData(), reinterpret_cast<const anyID*>(targetClientIDArray.GetData()), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelSubscribe(uint64 serverConnectionHandlerID, const TArray<uint64>& channelIDArray, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelSubscribe(serverConnectionHandlerID, channelIDArray.GetData(), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelSubscribeAll(uint64 serverConnectionHandlerID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelSubscribeAll(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelUnsubscribe(uint64 serverConnectionHandlerID, const TArray<uint64>& channelIDArray, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelUnsubscribe(serverConnectionHandlerID, channelIDArray.GetData(), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelUnsubscribeAll(uint64 serverConnectionHandlerID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelUnsubscribeAll(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestChannelDescription(uint64 serverConnectionHandlerID, uint64 channelID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestChannelDescription(serverConnectionHandlerID, channelID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestMuteClients(uint64 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestMuteClients(serverConnectionHandlerID, reinterpret_cast<const anyID*>(clientIDArray.GetData()), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestUnmuteClients(uint64 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestUnmuteClients(serverConnectionHandlerID, reinterpret_cast<const anyID*>(clientIDArray.GetData()), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestClientIDs(uint64 serverConnectionHandlerID, const FString& clientUniqueIdentifier, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestClientIDs(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*clientUniqueIdentifier), TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_requestSlotsFromProvisioningServer(const FString& ip, uint32 port, const FString& serverPassword, uint32 slots, const FString& identity, const FString& region) {
	unsigned int error;
	uint64 requestHandle;
	if ((error = ts3client_requestSlotsFromProvisioningServer(TS_TCHAR_TO_UTF8(*ip), port, TS_TCHAR_TO_UTF8(*serverPassword), slots, TS_TCHAR_TO_UTF8(*identity), TS_TCHAR_TO_UTF8(*region), &requestHandle)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(requestHandle, error);
}
FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_cancelRequestSlotsFromProvisioningServer(uint64 requestHandle) {
	unsigned int error;
	error = ts3client_cancelRequestSlotsFromProvisioningServer(requestHandle);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}
FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_startConnectionWithProvisioningKey(uint64 serverConnectionHandlerID, const FString& identity, const FString& nickname, const FString& connectionKey, const FString& clientMetaData) {
	unsigned int error;
	error = ts3client_startConnectionWithProvisioningKey(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*identity), TS_TCHAR_TO_UTF8(*nickname), TS_TCHAR_TO_UTF8(*connectionKey), TS_TCHAR_TO_UTF8(*clientMetaData));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getClientID(uint64 serverConnectionHandlerID) {
	unsigned int error;
	anyID result;
	if ((error = ts3client_getClientID(serverConnectionHandlerID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getConnectionStatus(uint64 serverConnectionHandlerID) {
	unsigned int error;
	int result;
	if ((error = ts3client_getConnectionStatus(serverConnectionHandlerID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getConnectionVariableAsUInt64(serverConnectionHandlerID, clientID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<float> TeamSpeak_Manager::UE_TS3_SDK_getConnectionVariableAsDouble(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag) {
	unsigned int error;
	double result;
	if ((error = ts3client_getConnectionVariableAsDouble(serverConnectionHandlerID, clientID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<float>(.0f, error);
	}
	return FTeamSpeak_valueHandler<float>(result, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getConnectionVariableAsString(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getConnectionVariableAsString(serverConnectionHandlerID, clientID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_cleanUpConnectionInfo(uint64 serverConnectionHandlerID, int32 clientID) {
	unsigned int error;
	error = ts3client_cleanUpConnectionInfo(serverConnectionHandlerID, clientID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestServerConnectionInfo(uint64 serverConnectionHandlerID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_requestServerConnectionInfo(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getServerConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getServerConnectionVariableAsUInt64(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<float> TeamSpeak_Manager::UE_TS3_SDK_getServerConnectionVariableAsFloat(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	float result;
	if ((error = ts3client_getServerConnectionVariableAsFloat(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<float>(.0f, error);
	}
	return FTeamSpeak_valueHandler<float>(result, error);
}


FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getClientSelfVariableAsInt(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	int result;
	if ((error = ts3client_getClientSelfVariableAsInt(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getClientSelfVariableAsString(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getClientSelfVariableAsString(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setClientSelfVariableAsInt(uint64 serverConnectionHandlerID, uint32 flag, int32 value) {
	unsigned int error;
	error = ts3client_setClientSelfVariableAsInt(serverConnectionHandlerID, flag, value);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setClientSelfVariableAsString(uint64 serverConnectionHandlerID, uint32 flag, const FString& value) {
	unsigned int error;
	error = ts3client_setClientSelfVariableAsString(serverConnectionHandlerID, flag, TS_TCHAR_TO_UTF8(*value));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_flushClientSelfUpdates(uint64 serverConnectionHandlerID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_flushClientSelfUpdates(serverConnectionHandlerID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getClientVariableAsInt(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag) {
	unsigned int error;
	int result;
	if ((error = ts3client_getClientVariableAsInt(serverConnectionHandlerID, clientID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getClientVariableAsUInt64(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getClientVariableAsUInt64(serverConnectionHandlerID, clientID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getClientVariableAsString(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getClientVariableAsString(serverConnectionHandlerID, clientID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	FString copy(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(copy, error);
}

FTeamSpeak_valueHandler<TArray<int32>> TeamSpeak_Manager::UE_TS3_SDK_getClientList(uint64 serverConnectionHandlerID) {
	unsigned int error;
	anyID* result;
	if ((error = ts3client_getClientList(serverConnectionHandlerID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<int32>>(TArray<int32>(), error);
	}
	TArray<uint32> buffer;
	for (int i = 0; result[i] != 0; ++i) {
		buffer.Insert(result[i], i);
	}
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<TArray<int32>>(static_cast<TArray<int32>>(buffer), error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getChannelOfClient(uint64 serverConnectionHandlerID, int32 clientID) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getChannelOfClient(serverConnectionHandlerID, clientID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag) {
	unsigned int error;
	int result;
	if ((error = ts3client_getChannelVariableAsInt(serverConnectionHandlerID, channelID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getChannelVariableAsUInt64(serverConnectionHandlerID, channelID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getChannelVariableAsString(serverConnectionHandlerID, channelID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}

    auto&& fstr = utf8_to_fstring(result);
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<FString>(fstr, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getChannelIDFromChannelNames(uint64 serverConnectionHandlerID, const TArray<FString>& channelNameArray) {
	unsigned int error;
	uint64 result;

    TArray<char*> wr_channel_name;
    if (channelNameArray.Num() > 0) {
        for (auto&& ch : channelNameArray) {
            auto&& conv_string = fstring_to_utf8(ch.GetCharArray().GetData());
            size_t count = conv_string.size() + 1;
            auto&& copy = new char[count];
            memset(copy, '\0', count);
#ifdef _WIN32
            strcpy_s(copy, count, conv_string.data());
#else
            strcpy(copy, conv_string.data());
#endif
            
            wr_channel_name.Push(copy);
        }

        auto&& e = new char[1];
        memset(e, '\0', 1);
        wr_channel_name.Push(e);
    }

	if ((error = ts3client_getChannelIDFromChannelNames(serverConnectionHandlerID, wr_channel_name.GetData(), &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}

    for (auto&& entry : wr_channel_name) {
        delete[] entry;
    }
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag, int32 value) {
	unsigned int error;
	error = ts3client_setChannelVariableAsInt(serverConnectionHandlerID, channelID, flag, value);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag, uint64 value) {
	unsigned int error;
	error = ts3client_setChannelVariableAsUInt64(serverConnectionHandlerID, channelID, flag, value);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_setChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag, const FString& value) {
	unsigned int error;
	error = ts3client_setChannelVariableAsString(serverConnectionHandlerID, channelID, flag, TS_TCHAR_TO_UTF8(*value));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_flushChannelUpdates(uint64 serverConnectionHandlerID, uint64 channelID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_flushChannelUpdates(serverConnectionHandlerID, channelID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_flushChannelCreation(uint64 serverConnectionHandlerID, uint64 channelParentID, const FString& returnCode) {
	unsigned int error;
	error = ts3client_flushChannelCreation(serverConnectionHandlerID, channelParentID, TS_TCHAR_TO_UTF8(*returnCode));
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

FTeamSpeak_valueHandler<TArray<int32>> TeamSpeak_Manager::UE_TS3_SDK_getChannelList(uint64 serverConnectionHandlerID) {
	unsigned int error;
	uint64* result;
	if ((error = ts3client_getChannelList(serverConnectionHandlerID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<int32>>(TArray<int32>(), error);
	}
	TArray<uint64> buffer;
	for (int i = 0; result[i] != 0; ++i) {
		buffer.Insert(result[i], i);
	}
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<TArray<int32>>(static_cast<TArray<int32>>(buffer), error);
}

FTeamSpeak_valueHandler<TArray<int32>> TeamSpeak_Manager::UE_TS3_SDK_getChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID) {
	unsigned int error;
	anyID* result;
	if ((error = ts3client_getChannelClientList(serverConnectionHandlerID, channelID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<int32>>(TArray<int32>(), error);
	}
	TArray<uint64> buffer;
	for (int i = 0; result[i] != 0; ++i) {
		buffer.Insert(result[i], i);
	}
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<TArray<int32>>(static_cast<TArray<int32>>(buffer), error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getParentChannelOfChannel(uint64 serverConnectionHandlerID, uint64 channelID) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getParentChannelOfChannel(serverConnectionHandlerID, channelID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getChannelEmptySecs(uint64 serverConnectionHandlerID, uint64 channelID) {
	unsigned int error;
	int result;
	if ((error = ts3client_getChannelEmptySecs(serverConnectionHandlerID, channelID, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<TArray<int32>> TeamSpeak_Manager::UE_TS3_SDK_getServerConnectionHandlerList() {
	unsigned int error;
	uint64* result;
	if ((error = ts3client_getServerConnectionHandlerList(&result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<TArray<int32>>(TArray<int32>(), error);
	}
	TArray<uint64> buffer;
	for (int i = 0; i < sizeof(result) / sizeof(uint64); ++i) {
		buffer.Insert(result[i], i);
	}
	ts3client_freeMemory(result);
	return FTeamSpeak_valueHandler<TArray<int32>>(static_cast<TArray<int32>>(buffer), error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getServerVariableAsInt(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	int result;
	if ((error = ts3client_getServerVariableAsInt(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<int32> TeamSpeak_Manager::UE_TS3_SDK_getServerVariableAsUInt64(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	uint64 result;
	if ((error = ts3client_getServerVariableAsUInt64(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<int32>(0, error);
	}
	return FTeamSpeak_valueHandler<int32>(result, error);
}

FTeamSpeak_valueHandler<FString> TeamSpeak_Manager::UE_TS3_SDK_getServerVariableAsString(uint64 serverConnectionHandlerID, uint32 flag) {
	unsigned int error;
	char* result;
	if ((error = ts3client_getServerVariableAsString(serverConnectionHandlerID, flag, &result)) != ERROR_ok) {
		return FTeamSpeak_valueHandler<FString>("", error);
	}
	return FTeamSpeak_valueHandler<FString>(result, error);
}

FTeamSpeak_valueHandler<void*> TeamSpeak_Manager::UE_TS3_SDK_requestServerVariables(uint64 serverConnectionHandlerID) {
	unsigned int error;
	error = ts3client_requestServerVariables(serverConnectionHandlerID);
	return FTeamSpeak_valueHandler<void*>(NULL, error);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//											Callback Events													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void TeamSpeak_Manager::onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onConnectStatusChangeEvent.Broadcast(serverConnectionHandlerID, static_cast<EConnectStatus>(newStatus), errorNumber);
}

void TeamSpeak_Manager::onServerProtocolVersionEvent(uint64 serverConnectionHandlerID, int protocolVersion) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onServerProtocolVersionEvent.Broadcast(serverConnectionHandlerID, protocolVersion);
}

void TeamSpeak_Manager::onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onNewChannelEvent.Broadcast(serverConnectionHandlerID, channelID, channelParentID);
}

void TeamSpeak_Manager::onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onNewChannelCreatedEvent.Broadcast(serverConnectionHandlerID, channelID, channelParentID, invokerID, TS_UTF8_TO_TCHAR(invokerName), TS_UTF8_TO_TCHAR(invokerUniqueIdentifier));
}

void TeamSpeak_Manager::onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onDelChannelEvent.Broadcast(serverConnectionHandlerID, channelID, invokerID, TS_UTF8_TO_TCHAR(invokerName), TS_UTF8_TO_TCHAR(invokerUniqueIdentifier));
}

void TeamSpeak_Manager::onChannelMoveEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelMoveEvent.Broadcast(serverConnectionHandlerID, channelID, newChannelParentID, invokerID, TS_UTF8_TO_TCHAR(invokerName), TS_UTF8_TO_TCHAR(invokerUniqueIdentifier));
}

void TeamSpeak_Manager::onUpdateChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onUpdateChannelEvent.Broadcast(serverConnectionHandlerID, channelID);
}

void TeamSpeak_Manager::onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onUpdateChannelEditedEvent.Broadcast(serverConnectionHandlerID, channelID, invokerID, TS_UTF8_TO_TCHAR(invokerName), TS_UTF8_TO_TCHAR(invokerUniqueIdentifier));
}

void TeamSpeak_Manager::onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onUpdateClientEvent.Broadcast(serverConnectionHandlerID, clientID, invokerID, TS_UTF8_TO_TCHAR(invokerName), TS_UTF8_TO_TCHAR(invokerUniqueIdentifier));
}

void TeamSpeak_Manager::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientMoveEvent.Broadcast(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, TS_UTF8_TO_TCHAR(moveMessage));
}

void TeamSpeak_Manager::onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientMoveSubscriptionEvent.Broadcast(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility);
}

void TeamSpeak_Manager::onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientMoveTimeoutEvent.Broadcast(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, TS_UTF8_TO_TCHAR(timeoutMessage));
}

void TeamSpeak_Manager::onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage) {
	TArray<FString> buffer;
	buffer.Push(moverName);
	buffer.Push(moverUniqueIdentifier);
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientMoveMovedEvent.Broadcast(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moverID, buffer, TS_UTF8_TO_TCHAR(moveMessage));
}

void TeamSpeak_Manager::onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
	TArray<FString> buffer;
	buffer.Push(kickerName);
	buffer.Push(kickerUniqueIdentifier);
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientKickFromChannelEvent.Broadcast(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, buffer, TS_UTF8_TO_TCHAR(kickMessage));
}

void TeamSpeak_Manager::onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage) {
	TArray<FString> buffer;
	buffer.Push(kickerName);
	buffer.Push(kickerUniqueIdentifier);
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientKickFromChannelEvent.Broadcast(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, buffer, TS_UTF8_TO_TCHAR(kickMessage));
}

void TeamSpeak_Manager::onClientIDsEvent(uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, anyID clientID, const char* clientName) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientIDsEvent.Broadcast(serverConnectionHandlerID, TS_UTF8_TO_TCHAR(uniqueClientIdentifier), clientID, TS_UTF8_TO_TCHAR(clientName));
}

void TeamSpeak_Manager::onClientIDsFinishedEvent(uint64 serverConnectionHandlerID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientIDsFinishedEvent.Broadcast(serverConnectionHandlerID);
}

void TeamSpeak_Manager::onServerEditedEvent(uint64 serverConnectionHandlerID, anyID editerID, const char* editerName, const char* editerUniqueIdentifier) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onServerEditedEvent.Broadcast(serverConnectionHandlerID, editerID, TS_UTF8_TO_TCHAR(editerName), TS_UTF8_TO_TCHAR(editerUniqueIdentifier));
}

void TeamSpeak_Manager::onServerUpdatedEvent(uint64 serverConnectionHandlerID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onServerUpdatedEvent.Broadcast(serverConnectionHandlerID);
}

void TeamSpeak_Manager::onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onServerErrorEvent.Broadcast(serverConnectionHandlerID, TS_UTF8_TO_TCHAR(errorMessage), error, TS_UTF8_TO_TCHAR(returnCode), TS_UTF8_TO_TCHAR(extraMessage));
}

void TeamSpeak_Manager::onServerStopEvent(uint64 serverConnectionHandlerID, const char* shutdownMessage) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onServerStopEvent.Broadcast(serverConnectionHandlerID, TS_UTF8_TO_TCHAR(shutdownMessage));
}

void TeamSpeak_Manager::onTextMessageEvent(uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onTextMessageEvent.Broadcast(serverConnectionHandlerID, targetMode, toID, fromID, TS_UTF8_TO_TCHAR(fromName), TS_UTF8_TO_TCHAR(fromUniqueIdentifier), TS_UTF8_TO_TCHAR(message));
}

void TeamSpeak_Manager::onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onTalkStatusChangeEvent.Broadcast(serverConnectionHandlerID, status, isReceivedWhisper, clientID);
}


void TeamSpeak_Manager::onIgnoredWhisperEvent(uint64 serverConnectionHandlerID, anyID clientID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onIgnoredWhisperEvent.Broadcast(serverConnectionHandlerID, clientID);
}

void TeamSpeak_Manager::onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onConnectionInfoEvent.Broadcast(serverConnectionHandlerID, clientID);
}

void TeamSpeak_Manager::onServerConnectionInfoEvent(uint64 serverConnectionHandlerID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onServerConnectionInfoEvent.Broadcast(serverConnectionHandlerID);
}

void TeamSpeak_Manager::onChannelSubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelSubscribeEvent.Broadcast(serverConnectionHandlerID, channelID);
}

void TeamSpeak_Manager::onChannelSubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelSubscribeFinishedEvent.Broadcast(serverConnectionHandlerID);
}

void TeamSpeak_Manager::onChannelUnsubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelUnsubscribeEvent.Broadcast(serverConnectionHandlerID, channelID);
}

void TeamSpeak_Manager::onChannelUnsubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelUnsubscribeFinishedEvent.Broadcast(serverConnectionHandlerID);
}

void TeamSpeak_Manager::onChannelDescriptionUpdateEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelDescriptionUpdateEvent.Broadcast(serverConnectionHandlerID, channelID);
}

void TeamSpeak_Manager::onChannelPasswordChangedEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onChannelPasswordChangedEvent.Broadcast(serverConnectionHandlerID, channelID);
}

void TeamSpeak_Manager::onPlaybackShutdownCompleteEvent(uint64 serverConnectionHandlerID) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onPlaybackShutdownCompleteEvent.Broadcast(serverConnectionHandlerID);
}

void TeamSpeak_Manager::onSoundDeviceListChangedEvent(const char* modeID, int playOrCap) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onSoundDeviceListChangedEvent.Broadcast(TS_UTF8_TO_TCHAR(modeID), playOrCap);
}

void TeamSpeak_Manager::onUserLoggingMessageEvent(const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onUserLoggingMessageEvent.Broadcast(TS_UTF8_TO_TCHAR(logmessage), logLevel, TS_UTF8_TO_TCHAR(logChannel), logID, TS_UTF8_TO_TCHAR(logTime), TS_UTF8_TO_TCHAR(completeLogString));
}

void TeamSpeak_Manager::onProvisioningSlotRequestResultEvent(unsigned int error, uint64 requestHandle, const char* connectionKey) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onProvisioningSlotRequestResultEvent.Broadcast(error, requestHandle, TS_UTF8_TO_TCHAR(connectionKey));
}

void TeamSpeak_Manager::onCheckServerUniqueIdentifierEvent(uint64 serverConnectionHandlerID, const char* ServerUniqueIdentifier, int* cancelConnect) {
	TArray<int32> buffer;
	for (int i = 0; i < (sizeof(cancelConnect) / sizeof(int)); ++i) {
		buffer.Push(cancelConnect[i]);
	}
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onCheckServerUniqueIdentifierEvent.Broadcast(serverConnectionHandlerID, TS_UTF8_TO_TCHAR(ServerUniqueIdentifier), buffer);
}

void TeamSpeak_Manager::onClientPasswordEncrypt(uint64 serverConnectionHandlerID, const char* plaintext, char* encryptedText, int encryptedTextByteSize) {
	UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary()->onClientPasswordEncrypt.Broadcast(serverConnectionHandlerID, TS_UTF8_TO_TCHAR(plaintext), TS_UTF8_TO_TCHAR(encryptedText), encryptedTextByteSize);
}

#include <string>
namespace converting_helper {
    TArray<FTeamSpeak_2dArray> conv_charppp_to_2d_array(char*** in) {
        auto kIn = in;
        TArray<FTeamSpeak_2dArray> out;
        for (int i = 0; *in; ++i) {
            TArray<FString> list;
            auto temp = *in;
            for (int o = 0; o < 2; ++o) {
                list.Insert(*temp++, o);
            }
            out.Insert(FTeamSpeak_2dArray(list), i);
            ++in;
        }
        return out;
    }

    TArray<FString> conv_charpp_to_array_fstring(char** kIn) {
        auto in = kIn;
        TArray<FString> out;
        for (int i = 0; *in; ++i) {
            out.Insert(*in++, i);
        }
        return out;
    }

    TArray<FString> conv_uint64p_to_array_fstring(const uint64* kIn) {
        auto in = kIn;
        TArray<FString> out;
        for (int i = 0; in[i]; ++i) {
            out.Insert(std::to_string(in[i]).data(), i);
        }
        return out;
    }
}