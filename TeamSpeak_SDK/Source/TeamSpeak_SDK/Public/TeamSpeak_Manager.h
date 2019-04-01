#pragma once

#ifdef __clang__
    #define TS_EXPORT __attribute__((visibility("default")))
#elif defined __WIN32
    #if WITH_TEAMSPEAK
	#define TS_EXPORT   __declspec(dllexport)
	#else
	#define TS_EXPORT   __declspec(dllimport)
	#endif
#endif

struct FTeamSpeak_2dArray;
enum class ETeamSpeak_Error;

using anyID = uint16_t;

template<typename T>
struct TS_EXPORT FTeamSpeak_valueHandler {
    T value;

    uint32 errorCode;

    T getValue() {
        return value;
    }
    uint32 getErrorCode() {
        return errorCode;
    }

    ETeamSpeak_Error getError() {
        return static_cast<ETeamSpeak_Error>(errorCode);
    }

    FTeamSpeak_valueHandler(T val, uint32 err) {
        errorCode = err;
        value = val;
    }
protected:
    FTeamSpeak_valueHandler() = delete;
};

typedef struct
{
private:
    using OnEditPlaybackVoiceDataEventPtr = void(*)(uint64, anyID, short*, int, int);
    using OnEditPostProcessVoiceDataEventPtr = void(*)(uint64, anyID, short*, int, int, const unsigned int*, unsigned int*);
    using OnEditMixedPlaybackVoiceDataEventPtr = void(*)(uint64, short*, int, int, const unsigned int*, unsigned int*);
    using OnEditCapturedVoiceDataEventPtr = void(*)(uint64, short*, int, int, int*);
    using OnCustom3dRolloffCalculationClientEventPtr = void(*)(uint64, anyID, float, float*);
    using OnCustom3dRolloffCalculationWaveEventPtr = void(*)(uint64, uint64, float, float*);

public:
    OnEditPlaybackVoiceDataEventPtr on_edit_playback_voice_data_ptr = nullptr;
    OnEditPostProcessVoiceDataEventPtr on_edit_post_process_voide_data_ptr = nullptr;
    OnEditMixedPlaybackVoiceDataEventPtr on_edit_mixed_playback_voide_data_ptr = nullptr;
    OnEditCapturedVoiceDataEventPtr on_edit_captured_voice_data_ptr = nullptr;
    OnCustom3dRolloffCalculationClientEventPtr on_custom_3d_rolloff_calc_client_ptr = nullptr;
    OnCustom3dRolloffCalculationWaveEventPtr on_custom_3d_rollof_calc_wave_ptr = nullptr;
} Custom_Function_Function_Pointer;

class TS_EXPORT TeamSpeak_Manager {
public:
    TeamSpeak_Manager() = default;
    virtual ~TeamSpeak_Manager() = default;

    static TeamSpeak_Manager* get_instance();

	/*Construction and Destruction*/
    FTeamSpeak_valueHandler<void*> UE_TS3_SDK_initClientLib(int32 usedLogTypes, const FString& logFileFolder, const FString& resourcesFolder, const Custom_Function_Function_Pointer* const custom_function_ptr = nullptr);
    FTeamSpeak_valueHandler<void*> UE_TS3_SDK_initClientLib(int32 usedLogTypes, const FString& logFileFolder, const FString& resourcesFolder, const struct ClientUIFunctions& callbacks);

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_destroyClientLib();
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getClientLibVersion();
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getClientLibVersionNumber();
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_spawnNewServerConnectionHandler(int32 port);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_destroyServerConnectionHandler(uint64 serverConnectionHandlerID);

	/*Identity management*/
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_createIdentity();
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_identityStringToUniqueIdentifier(const FString& identityString);

	/*sound*/
	FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>> UE_TS3_SDK_getPlaybackDeviceList(const FString& modeID);
	FTeamSpeak_valueHandler<TArray<FTeamSpeak_2dArray>> UE_TS3_SDK_getCaptureDeviceList(const FString& modeID);
	FTeamSpeak_valueHandler<TArray<FString>> UE_TS3_SDK_getPlaybackModeList();
	FTeamSpeak_valueHandler<TArray<FString>> UE_TS3_SDK_getCaptureModeList();
	FTeamSpeak_valueHandler<TArray<FString>> UE_TS3_SDK_getDefaultPlaybackDevice(const FString& modeID);
	FTeamSpeak_valueHandler<TArray<FString>> UE_TS3_SDK_getDefaultCaptureDevice(const FString& modeID);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getDefaultPlayBackMode();
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getDefaultCaptureMode();

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_openPlaybackDevice(uint64 serverConnectionHandlerID, const FString& modeID, const FString& playbackDevice);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_openCaptureDevice(uint64 serverConnectionHandlerID, const FString& modeID, const FString& captureDevice);
	FTeamSpeak_valueHandler<TArray<FString>> UE_TS3_SDK_getCurrentPlaybackDeviceName(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getCurrentPlayBackMode(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<TArray<FString>> UE_TS3_SDK_getCurrentCaptureDeviceName(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getCurrentCaptureMode(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_initiateGracefulPlaybackShutdown(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_closePlaybackDevice(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_closeCaptureDevice(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_activateCaptureDevice(uint64 serverConnectionHandlerID);

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_playWaveFile(uint64 serverConnectionHandlerID, const FString& path);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_playWaveFileHandle(uint64 serverConnectionHandlerID, const FString& path, int32 loop, uint64 waveHandle);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_pauseWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle, int32 pause);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_closeWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle);

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_registerCustomDevice(const FString& deviceID, const FString& deviceDisplayName, int32 capFrequency, int32 capChannels, int32 playFrequency, int32 playChannels);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_unregisterCustomDevice(const FString& deviceID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_processCustomCaptureData(const FString& deviceName, const TArray<int16>& buffer, int32 samples);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_acquireCustomPlaybackData(const FString& deviceName, TArray<int32>& buffer, int32 samples);

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setLocalTestMode(uint64 serverConnectionHandlerID, int32 status);

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_startVoiceRecording(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_stopVoiceRecording(uint64 serverConnectionHandlerID);

	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_allowWhispersFrom(uint64 serverConnectionHandlerID, int32 clID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_removeFromAllowedWhispersFrom(uint64 serverConnectionHandlerID, int32 clID);

	/* 3d sound positioning */
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_systemset3DListenerAttributes(uint64 serverConnectionHandlerID, FVector position, FVector forward, FVector up);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_set3DWaveAttributes(uint64 serverConnectionHandlerID, uint64 waveHandle, FVector position);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_systemset3DSettings(uint64 serverConnectionHandlerID, float distanceFactor, float rolloffScale);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_channelset3DAttributes(uint64 serverConnectionHandlerID, int32 clientID, FVector position);

	/*preprocessor*/
	FTeamSpeak_valueHandler<float> UE_TS3_SDK_getPreProcessorInfoValueFloat(uint64 serverConnectionHandlerID, const FString& ident);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getPreProcessorConfigValue(uint64 serverConnectionHandlerID, const FString& ident);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setPreProcessorConfigValue(uint64 serverConnectionHandlerID, const FString& ident, const FString& value);

	/*encoder*/
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getEncodeConfigValue(uint64 serverConnectionHandlerID, const FString& ident);

	/*playback*/
	FTeamSpeak_valueHandler<float> UE_TS3_SDK_getPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const FString& ident);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setPlaybackConfigValue(uint64 serverConnectionHandlerID, const FString& ident, const FString& value);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setClientVolumeModifier(uint64 serverConnectionHandlerID, int32 clientID, float value);

	/*logging*/
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_logMessage(const FString& logMessage, int32 severity, const FString& channel, uint64 logID);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setLogVerbosity(int32 logVerbosity);

	/*error handling*/
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getErrorMessage(int32 errorCode);

	/*Interacting with the server*/
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_startConnection(uint64 serverConnectionHandlerID, const FString& identity, const FString& ip, int32 port, const FString& nickname, const TArray<FString>& defaultChannelArray, const FString& defaultChannelPassword, const FString& serverPassword);
    FTeamSpeak_valueHandler<void*> UE_TS3_SDK_startConnectionWithChannelID(uint64 serverConnectionHandlerID, const FString& identity, const FString& ip, unsigned int port, const FString& nickname, uint64 defaultChannelId, const FString& defaultChannelPassword, const FString& serverPassword);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_stopConnection(uint64 serverConnectionHandlerID, const FString& quitMessage);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestClientMove(uint64 serverConnectionHandlerID, int32 clientID, uint64 newChannelID, const FString& password, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestClientVariables(uint64 serverConnectionHandlerID, int32 clientID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestClientKickFromChannel(uint64 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestClientKickFromServer(uint64 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelDelete(uint64 serverConnectionHandlerID, uint64 channelID, int32 force, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelMove(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, uint64 newChannelOrder, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestSendPrivateTextMsg(uint64 serverConnectionHandlerID, const FString& message, int32 targetClientID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestSendChannelTextMsg(uint64 serverConnectionHandlerID, const FString& message, uint64 targetChannelID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestSendServerTextMsg(uint64 serverConnectionHandlerID, const FString& message, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestConnectionInfo(uint64 serverConnectionHandlerID, int32 clientID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestClientSetWhisperList(uint64 serverConnectionHandlerID, int32 clientID, const TArray<uint64>& targetChannelIDArray, const TArray<int32>& targetClientIDArray, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelSubscribe(uint64 serverConnectionHandlerID, const TArray<uint64>& channelIDArray, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelSubscribeAll(uint64 serverConnectionHandlerID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelUnsubscribe(uint64 serverConnectionHandlerID, const TArray<uint64>& channelIDArray, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelUnsubscribeAll(uint64 serverConnectionHandlerID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestChannelDescription(uint64 serverConnectionHandlerID, uint64 channelID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestMuteClients(uint64 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestUnmuteClients(uint64 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestClientIDs(uint64 serverConnectionHandlerID, const FString& clientUniqueIdentifier, const FString& returnCode);

	/*provisioning server calls*/
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_requestSlotsFromProvisioningServer(const FString& ip, uint32 port, const FString& serverPassword, uint32 slots, const FString& identity, const FString& region);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_cancelRequestSlotsFromProvisioningServer(uint64 requestHandle);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_startConnectionWithProvisioningKey(uint64 serverConnectionHandlerID, const FString& identity, const FString& nickname, const FString& connectionKey, const FString& clientMetaData);

	/*retrieve information ClientLib has stored*/

	/*general info*/
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getClientID(uint64 serverConnectionHandlerID);

	/*client connection info*/
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getConnectionStatus(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag);
	FTeamSpeak_valueHandler<float> UE_TS3_SDK_getConnectionVariableAsDouble(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getConnectionVariableAsString(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_cleanUpConnectionInfo(uint64 serverConnectionHandlerID, int32 clientID);

	/*server connection info*/
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestServerConnectionInfo(uint64 serverConnectionHandlerID, const FString& returnCode);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getServerConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, uint32 flag);
	FTeamSpeak_valueHandler<float> UE_TS3_SDK_getServerConnectionVariableAsFloat(uint64 serverConnectionHandlerID, uint32 flag);

	/*client info*/
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getClientSelfVariableAsInt(uint64 serverConnectionHandlerID, uint32 flag);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getClientSelfVariableAsString(uint64 serverConnectionHandlerID, uint32 flag);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setClientSelfVariableAsInt(uint64 serverConnectionHandlerID, uint32 flag, int32 value);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setClientSelfVariableAsString(uint64 serverConnectionHandlerID, uint32 flag, const FString& value);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_flushClientSelfUpdates(uint64 serverConnectionHandlerID, const FString& returnCode);

	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getClientVariableAsInt(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getClientVariableAsUInt64(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getClientVariableAsString(uint64 serverConnectionHandlerID, int32 clientID, uint32 flag);

	FTeamSpeak_valueHandler<TArray<int32>> UE_TS3_SDK_getClientList(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getChannelOfClient(uint64 serverConnectionHandlerID, int32 clientID);

	/*channel info*/
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag);

	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getChannelIDFromChannelNames(uint64 serverConnectionHandlerID, const TArray<FString>& channelNameArray);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag, int32 value);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag, uint64 value);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_setChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, uint32 flag, const FString& value);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_flushChannelUpdates(uint64 serverConnectionHandlerID, uint64 channelID, const FString& returnCode);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_flushChannelCreation(uint64 serverConnectionHandlerID, uint64 channelParentID, const FString& returnCode);

	FTeamSpeak_valueHandler<TArray<int32>> UE_TS3_SDK_getChannelList(uint64 serverConnectionHandlerID);
	FTeamSpeak_valueHandler<TArray<int32>> UE_TS3_SDK_getChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getParentChannelOfChannel(uint64 serverConnectionHandlerID, uint64 channelID);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getChannelEmptySecs(uint64 serverConnectionHandlerID, uint64 channelID);

	/*server info*/
	FTeamSpeak_valueHandler<TArray<int32>> UE_TS3_SDK_getServerConnectionHandlerList();
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getServerVariableAsInt(uint64 serverConnectionHandlerID, uint32 flag);
	FTeamSpeak_valueHandler<int32> UE_TS3_SDK_getServerVariableAsUInt64(uint64 serverConnectionHandlerID, uint32 flag);
	FTeamSpeak_valueHandler<FString> UE_TS3_SDK_getServerVariableAsString(uint64 serverConnectionHandlerID, uint32 flag);
	FTeamSpeak_valueHandler<void*> UE_TS3_SDK_requestServerVariables(uint64 serverConnectionHandlerID);



private:
	static void onConnectStatusChangeEvent              (uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
	static void onServerProtocolVersionEvent            (uint64 serverConnectionHandlerID, int protocolVersion);
	static void onNewChannelEvent                       (uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID);
	static void onNewChannelCreatedEvent                (uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	static void onDelChannelEvent                       (uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	static void onChannelMoveEvent                      (uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	static void onUpdateChannelEvent                    (uint64 serverConnectionHandlerID, uint64 channelID);
	static void onUpdateChannelEditedEvent              (uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	static void onUpdateClientEvent                     (uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	static void onClientMoveEvent                       (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage);
	static void onClientMoveSubscriptionEvent           (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);
	static void onClientMoveTimeoutEvent                (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage);
	static void onClientMoveMovedEvent                  (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage);
	static void onClientKickFromChannelEvent            (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);
	static void onClientKickFromServerEvent             (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);
	static void onClientIDsEvent                        (uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, anyID clientID, const char* clientName);
	static void onClientIDsFinishedEvent                (uint64 serverConnectionHandlerID);
	static void onServerEditedEvent                     (uint64 serverConnectionHandlerID, anyID editerID, const char* editerName, const char* editerUniqueIdentifier);
	static void onServerUpdatedEvent                    (uint64 serverConnectionHandlerID);
	static void onServerErrorEvent                      (uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
	static void onServerStopEvent                       (uint64 serverConnectionHandlerID, const char* shutdownMessage);
	static void onTextMessageEvent                      (uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message);
	static void onTalkStatusChangeEvent                 (uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
	static void onIgnoredWhisperEvent                   (uint64 serverConnectionHandlerID, anyID clientID);
	static void onConnectionInfoEvent                   (uint64 serverConnectionHandlerID, anyID clientID);
	static void onServerConnectionInfoEvent             (uint64 serverConnectionHandlerID);
	static void onChannelSubscribeEvent                 (uint64 serverConnectionHandlerID, uint64 channelID);
	static void onChannelSubscribeFinishedEvent         (uint64 serverConnectionHandlerID);
	static void onChannelUnsubscribeEvent               (uint64 serverConnectionHandlerID, uint64 channelID);
	static void onChannelUnsubscribeFinishedEvent       (uint64 serverConnectionHandlerID);
	static void onChannelDescriptionUpdateEvent         (uint64 serverConnectionHandlerID, uint64 channelID);
	static void onChannelPasswordChangedEvent           (uint64 serverConnectionHandlerID, uint64 channelID);
	static void onPlaybackShutdownCompleteEvent         (uint64 serverConnectionHandlerID);
	static void onSoundDeviceListChangedEvent           (const char* modeID, int playOrCap);
	static void onUserLoggingMessageEvent               (const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
	static void onProvisioningSlotRequestResultEvent    (unsigned int error, uint64 requestHandle, const char* connectionKey);
	static void onCheckServerUniqueIdentifierEvent      (uint64 serverConnectionHandlerID, const char* ServerUniqueIdentifier, int* cancelConnect);
	static void onClientPasswordEncrypt                 (uint64 serverConnectionHandlerID, const char* plaintext, char* encryptedText, int encryptedTextByteSize);
};

namespace converting_helper
{
    TArray<FTeamSpeak_2dArray> conv_charppp_to_2d_array(char*** in);
    TArray<FString> conv_charpp_to_array_fstring(char** kIn);
    TArray<FString> conv_uint64p_to_array_fstring(const uint64* kIn);
}
