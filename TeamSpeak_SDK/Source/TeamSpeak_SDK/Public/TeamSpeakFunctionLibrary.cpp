#include "TeamSpeak_SDK.h"
#include "TeamSpeak_Manager.h"
#include "TeamSpeakFunctionLibrary.h"

UTeamSpeakFunctionLibrary* UTeamSpeakFunctionLibrary::utsFL = nullptr;
TSharedPtr<TeamSpeak_Manager> UTeamSpeakFunctionLibrary::ts_Manager = MakeShareable(new TeamSpeak_Manager);

UTeamSpeakFunctionLibrary::UTeamSpeakFunctionLibrary(const class FObjectInitializer& PCIP) 
	: Super(PCIP) {
	utsFL = this;
}

void UTeamSpeakFunctionLibrary::printMessageDebug(FString str) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, str);
}

FString UTeamSpeakFunctionLibrary::getSoundBackendDir_Editor() {
	return FPaths::GamePluginsDir() + "TeamSpeak_SDK/ThirdParty/TeamSpeak_lib/";
}

UTeamSpeakFunctionLibrary* UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary() {
    return utsFL;
}

TeamSpeak_Manager* UTeamSpeakFunctionLibrary::get_teampspeak_manager_ptr()
{
    return ts_Manager.Get();
}


void UTeamSpeakFunctionLibrary::TeamSpeak_initClientLib(ETeamSpeak_LogTypes usedLogTypes, const FString& logFileFolder, const FString& resourcesFolder, int32 &error){
	auto handler = ts_Manager->UE_TS3_SDK_initClientLib(static_cast<uint32>(usedLogTypes), logFileFolder, resourcesFolder);
	error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_destroyClientLib(int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_destroyClientLib();
	error = handler.getErrorCode();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getClientLibVersion(int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getClientLibVersion();
    error = handler.getErrorCode();
	return handler.getValue();
}

int32 UTeamSpeakFunctionLibrary::TeamSpeak_getClientLibVersionNumber(int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getClientLibVersionNumber();
    error = handler.getErrorCode();
	return handler.getValue();
}

int32 UTeamSpeakFunctionLibrary::TeamSpeak_spawnNewServerConnectionHandler(int32 port, int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_spawnNewServerConnectionHandler(port);
    error = handler.getErrorCode();
	return handler.getValue();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_destroyServerConnectionHandler(int32 serverConnectionHandlerID, int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_destroyServerConnectionHandler(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_createIdentity(int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_createIdentity();
    error = handler.getErrorCode();
	return handler.getValue();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_identityStringToUniqueIdentifier(const FString& identityString, int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_identityStringToUniqueIdentifier(identityString);
    error = handler.getErrorCode();
	return handler.getValue();
}

TArray<FTeamSpeak_2dArray> UTeamSpeakFunctionLibrary::TeamSpeak_getPlaybackDeviceList(const FString& modeID, int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getPlaybackDeviceList(modeID);
    error = handler.getErrorCode();
	return handler.getValue();
}

TArray<FTeamSpeak_2dArray> UTeamSpeakFunctionLibrary::TeamSpeak_getCaptureDeviceList(const FString& modeID, int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getCaptureDeviceList(modeID);
    error = handler.getErrorCode();
	return handler.getValue();
}

TArray<FString> UTeamSpeakFunctionLibrary::TeamSpeak_getPlaybackModeList(int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getPlaybackModeList();
    error = handler.getErrorCode();
	return handler.getValue();
}

TArray<FString> UTeamSpeakFunctionLibrary::TeamSpeak_getCaptureModeList(int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getCaptureModeList();
    error = handler.getErrorCode();
	return handler.getValue();
}

TArray<FString> UTeamSpeakFunctionLibrary::TeamSpeak_getDefaultPlaybackDevice(const FString& modeID, int32 &error) {
    auto handler = ts_Manager->UE_TS3_SDK_getDefaultPlaybackDevice(modeID);
    error = handler.getErrorCode();
	return handler.getValue();
}

TArray<FString> UTeamSpeakFunctionLibrary::TeamSpeak_getDefaultCaptureDevice(const FString& modeID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getDefaultCaptureDevice(modeID);
    error = handler.getErrorCode();
	return handler.getValue();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getDefaultPlayBackMode(int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getDefaultPlayBackMode();
    error = handler.getErrorCode();
	return handler.getValue();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getDefaultCaptureMode(int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getDefaultCaptureMode();
    error = handler.getErrorCode();
	return handler.getValue();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_openPlaybackDevice(int32 serverConnectionHandlerID, const FString& modeID, const FString& playbackDevice, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_openPlaybackDevice(serverConnectionHandlerID, modeID, playbackDevice);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_openCaptureDevice(int32 serverConnectionHandlerID, const FString& modeID, const FString& captureDevice, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_openCaptureDevice(serverConnectionHandlerID, modeID, captureDevice);
    error = handler.getErrorCode();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getCurrentPlaybackDeviceName(int32 serverConnectionHandlerID, int32 &error, int32 &isDefualt) {
	auto handler = ts_Manager->UE_TS3_SDK_getCurrentPlaybackDeviceName(serverConnectionHandlerID);
    error = handler.getErrorCode();
	isDefualt = FCString::Atoi(*handler.getValue().Last());
	return handler.getValue().Pop();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getCurrentPlayBackMode(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getCurrentPlayBackMode(serverConnectionHandlerID);
    error = handler.getErrorCode();
	return handler.getValue();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getCurrentCaptureDeviceName(int32 serverConnectionHandlerID, int32 &error, int32 &isDefault) {
	auto handler = ts_Manager->UE_TS3_SDK_getCurrentCaptureDeviceName(serverConnectionHandlerID);
    error = handler.getErrorCode();
	isDefault = FCString::Atoi(*handler.getValue().Last());
	return handler.getValue().Pop();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getCurrentCaptureMode(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getCurrentCaptureMode(serverConnectionHandlerID);
    error = handler.getErrorCode();
	return handler.getValue();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_initiateGracefulPlaybackShutdown(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_initiateGracefulPlaybackShutdown(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_closePlaybackDevice(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_closePlaybackDevice(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_closeCaptureDevice(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_closeCaptureDevice(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_activateCaptureDevice(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_activateCaptureDevice(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_playWaveFile(int32 serverConnectionHandlerID, const FString& path, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_playWaveFile(serverConnectionHandlerID, path);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_playWaveFileHandle(int32 serverConnectionHandlerID, const FString& path, int32 loop, int32 waveHandle, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_playWaveFileHandle(serverConnectionHandlerID, path, loop, waveHandle);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_pauseWaveFileHandle(int32 serverConnectionHandlerID, int32 waveHandle, int32 pause, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_pauseWaveFileHandle(serverConnectionHandlerID, waveHandle, pause);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_closeWaveFileHandle(int32 serverConnectionHandlerID, int32 waveHandle, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_closeWaveFileHandle(serverConnectionHandlerID, waveHandle);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_registerCustomDevice(const FString& deviceID, const FString& deviceDisplayName, int32 capFrequency, int32 capChannels, int32 playFrequency, int32 playChannels, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_registerCustomDevice(deviceID, deviceDisplayName, capFrequency, capChannels, playFrequency, playChannels);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_unregisterCustomDevice(const FString& deviceID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_unregisterCustomDevice(deviceID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_processCustomCaptureData(const FString& deviceName, const TArray<int32>& buffer, int32 samples, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_processCustomCaptureData(deviceName, static_cast<TArray<int16>>(buffer), samples);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_acquireCustomPlaybackData(const FString& deviceName, TArray<int32>& buffer, int32 samples, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_acquireCustomPlaybackData(deviceName, buffer, samples);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_setLocalTestMode(int32 serverConnectionHandlerID, int32 status, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setLocalTestMode(serverConnectionHandlerID, status);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_startVoiceRecording(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_startVoiceRecording(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_stopVoiceRecording(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_stopVoiceRecording(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_allowWhispersFrom(int32 serverConnectionHandlerID, int32 clID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_allowWhispersFrom(serverConnectionHandlerID, clID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_removeFromAllowedWhispersFrom(int32 serverConnectionHandlerID, int32 clID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_removeFromAllowedWhispersFrom(serverConnectionHandlerID, clID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_systemset3DListenerAttributes(int32 serverConnectionHandlerID, FVector position, FVector forward, FVector up, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_systemset3DListenerAttributes(serverConnectionHandlerID, position, forward, up);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_set3DWaveAttributes(int32 serverConnectionHandlerID, int32 waveHandle, FVector position, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_set3DWaveAttributes(serverConnectionHandlerID, waveHandle, position);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_systemset3DSettings(int32 serverConnectionHandlerID, float distanceFactor, float rolloffScale, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_systemset3DSettings(serverConnectionHandlerID, distanceFactor, rolloffScale);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_channelset3DAttributes(int32 serverConnectionHandlerID, int32 clientID, FVector position, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_channelset3DAttributes(serverConnectionHandlerID, clientID, position);
    error = handler.getErrorCode();
}

float UTeamSpeakFunctionLibrary::TeamSpeak_getPreProcessorInfoValueFloat(int32 serverConnectionHandlerID, const FString& ident, int32 &error){
	auto handler = ts_Manager->UE_TS3_SDK_getPreProcessorInfoValueFloat(serverConnectionHandlerID, ident);
    error = handler.getErrorCode();
	return handler.getValue();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getPreProcessorConfigValue(int32 serverConnectionHandlerID, const FString& ident, int32 &error){
	auto handler = ts_Manager->UE_TS3_SDK_getPreProcessorConfigValue(serverConnectionHandlerID, ident);
    error = handler.getErrorCode();
	return handler.getValue();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_setPreProcessorConfigValue(int32 serverConnectionHandlerID, const FString& ident, const FString& value, int32 &error){
	auto handler = ts_Manager->UE_TS3_SDK_setPreProcessorConfigValue(serverConnectionHandlerID, ident, value);
    error = handler.getErrorCode();
}

FString UTeamSpeakFunctionLibrary::TeamSpeak_getEncodeConfigValu(int32 serverConnectionHandlerID, const FString& ident, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getEncodeConfigValue(serverConnectionHandlerID, ident);
    error = handler.getErrorCode();
	return handler.getValue();
}

float UTeamSpeakFunctionLibrary::TeamSpeak_getPlaybackConfigValueAsFloat(int32 serverConnectionHandlerID, const FString& ident, int32 &error) { 
	auto handler = ts_Manager->UE_TS3_SDK_getPlaybackConfigValueAsFloat(serverConnectionHandlerID, ident);
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setPlaybackConfigValue(int32 serverConnectionHandlerID, const FString& ident, const FString& value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setPlaybackConfigValue(serverConnectionHandlerID, ident, value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setClientVolumeModifier(int32 serverConnectionHandlerID, int32 clientID, float value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setClientVolumeModifier(serverConnectionHandlerID, clientID, value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_logMessage(const FString& logMessage, int32 severity, const FString& channel, int32 logID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_logMessage(logMessage, severity, channel, logID);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setLogVerbosity(int32 logVerbosity, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setLogVerbosity(logVerbosity);
    error = handler.getErrorCode();
}
FString UTeamSpeakFunctionLibrary::TeamSpeak_getErrorMessage(int32 errorCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getErrorMessage(errorCode);
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_startConnection(int32 serverConnectionHandlerID, const FString& identity, const FString& ip, int32 port, const FString& nickname, const TArray<FString>& defaultChannelArray, const FString& defaultChannelPassword, const FString& serverPassword, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_startConnection(serverConnectionHandlerID, identity, ip, port, nickname, defaultChannelArray, defaultChannelPassword, serverPassword);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_startConnectionWithChannelID(int32 serverConnectionHandlerID, const FString& identity, const FString& ip, int32 port, const FString& nickname, int32 defaultChannelId, const FString& defaultChannelPassword, const FString& serverPassword, int32& error) {
    auto handler = ts_Manager->UE_TS3_SDK_startConnectionWithChannelID(serverConnectionHandlerID, identity, ip, port, nickname, defaultChannelId, defaultChannelPassword, serverPassword);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_stopConnection(int32 serverConnectionHandlerID, const FString& quitMessage, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_stopConnection(serverConnectionHandlerID, quitMessage);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestClientMove(int32 serverConnectionHandlerID, int32 clientID, int32 newChannelID, const FString& password, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestClientMove(serverConnectionHandlerID, clientID, newChannelID, password, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestClientVariables(int32 serverConnectionHandlerID, int32 clientID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestClientVariables(serverConnectionHandlerID, clientID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestClientKickFromChannel(int32 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestClientKickFromChannel(serverConnectionHandlerID, clientID, kickReason, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestClientKickFromServer(int32 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestClientKickFromServer(serverConnectionHandlerID, clientID, kickReason, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelDelete(int32 serverConnectionHandlerID, int32 channelID, int32 force, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelDelete(serverConnectionHandlerID, channelID, force, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelMove(int32 serverConnectionHandlerID, int32 channelID, int32 newChannelParentID, int32 newChannelOrder, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelMove(serverConnectionHandlerID, channelID, newChannelParentID, newChannelOrder, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestSendPrivateTextMsg(int32 serverConnectionHandlerID, const FString& message, int32 targetClientID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestSendPrivateTextMsg(serverConnectionHandlerID, message, targetClientID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestSendChannelTextMsg(int32 serverConnectionHandlerID, const FString& message, int32 targetChannelID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestSendChannelTextMsg(serverConnectionHandlerID, message, targetChannelID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestSendServerTextMsg(int32 serverConnectionHandlerID, const FString& message, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestSendServerTextMsg(serverConnectionHandlerID, message, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestConnectionInfo(int32 serverConnectionHandlerID, int32 clientID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestConnectionInfo(serverConnectionHandlerID, clientID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestClientSetWhisperList(int32 serverConnectionHandlerID, int32 clientID, const TArray<int32>& targetChannelIDArray, const TArray<int32>& targetClientIDArray, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestClientSetWhisperList(serverConnectionHandlerID, clientID, static_cast<TArray<uint64>>(targetChannelIDArray), targetClientIDArray, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelSubscribe(int32 serverConnectionHandlerID, const TArray<int32>& channelIDArray, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelSubscribe(serverConnectionHandlerID, static_cast<TArray<uint64>>(channelIDArray), returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelSubscribeAll(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelSubscribeAll(serverConnectionHandlerID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelUnsubscribe(int32 serverConnectionHandlerID, const TArray<int32>& channelIDArray, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelUnsubscribe(serverConnectionHandlerID, static_cast<TArray<uint64>>(channelIDArray), returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelUnsubscribeAll(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelUnsubscribeAll(serverConnectionHandlerID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestChannelDescription(int32 serverConnectionHandlerID, int32 channelID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestChannelDescription(serverConnectionHandlerID, channelID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestMuteClients(int32 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestMuteClients(serverConnectionHandlerID, clientIDArray, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestUnmuteClients(int32 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestUnmuteClients(serverConnectionHandlerID, clientIDArray, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestClientIDs(int32 serverConnectionHandlerID, const FString& clientUniqueIdentifier, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestClientIDs(serverConnectionHandlerID, clientUniqueIdentifier, returnCode);
    error = handler.getErrorCode();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_requestSlotsFromProvisioningServer(const FString& ip, int32 port, const FString& serverPassword, int32 slots, const FString& identity, const FString& region, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestSlotsFromProvisioningServer(ip, port, serverPassword, slots, identity, region);
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_cancelRequestSlotsFromProvisioningServer(int32 requestHandle, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_cancelRequestSlotsFromProvisioningServer(requestHandle);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_startConnectionWithProvisioningKey(int32 serverConnectionHandlerID, const FString& identity, const FString& nickname, const FString& connectionKey, const FString& clientMetaData, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_startConnectionWithProvisioningKey(serverConnectionHandlerID, identity, nickname, connectionKey, clientMetaData);
    error = handler.getErrorCode();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getClientID(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getClientID(serverConnectionHandlerID);
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getConnectionStatus(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getConnectionStatus(serverConnectionHandlerID);
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_cleanUpConnectionInfo(int32 serverConnectionHandlerID, int32 clientID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_cleanUpConnectionInfo(serverConnectionHandlerID, clientID);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestServerConnectionInfo(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestServerConnectionInfo(serverConnectionHandlerID, returnCode);
    error = handler.getErrorCode();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getClientSelfVariableAsint32(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getClientSelfVariableAsInt(serverConnectionHandlerID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
FString UTeamSpeakFunctionLibrary::TeamSpeak_getClientSelfVariableAsString(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getClientSelfVariableAsString(serverConnectionHandlerID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setClientSelfVariableAsint32(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, int32 value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setClientSelfVariableAsInt(serverConnectionHandlerID, static_cast<int32>(flag), value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setClientSelfVariableAsString(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, const FString& value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setClientSelfVariableAsString(serverConnectionHandlerID, static_cast<int32>(flag), value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_flushClientSelfUpdates(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_flushClientSelfUpdates(serverConnectionHandlerID, returnCode);
    error = handler.getErrorCode();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getClientVariableAsInt(int32 serverConnectionHandlerID, int32 clientID, ETeamSpeak_ClientProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getClientVariableAsInt(serverConnectionHandlerID, clientID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getClientVariableAsUint64(int32 serverConnectionHandlerID, int32 clientID, ETeamSpeak_ClientProperties flag, int32 &error) {
	 
	auto handler = ts_Manager->UE_TS3_SDK_getClientVariableAsUInt64(serverConnectionHandlerID, clientID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
FString UTeamSpeakFunctionLibrary::TeamSpeak_getClientVariableAsString(int32 serverConnectionHandlerID, int32 clientID, ETeamSpeak_ClientProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getClientVariableAsString(serverConnectionHandlerID, clientID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
TArray<int32> UTeamSpeakFunctionLibrary::TeamSpeak_getClientList(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getClientList(serverConnectionHandlerID);
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getChannelOfClient(int32 serverConnectionHandlerID, int32 clientID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelOfClient(serverConnectionHandlerID, clientID);
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getChannelVariableAsInt(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelVariableAsInt(serverConnectionHandlerID, channelID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getChannelVariableAsUint64(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelVariableAsUInt64(serverConnectionHandlerID, channelID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
FString UTeamSpeakFunctionLibrary::TeamSpeak_getChannelVariableAsString(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelVariableAsString(serverConnectionHandlerID, channelID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getChannelIDFromChannelNames(int32 serverConnectionHandlerID, const TArray<FString>& channelNameArray, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelIDFromChannelNames(serverConnectionHandlerID, channelNameArray);
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setChannelVariableAsInt(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setChannelVariableAsInt(serverConnectionHandlerID, channelID, static_cast<int32>(flag), value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setChannelVariableAsUint64(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setChannelVariableAsUInt64(serverConnectionHandlerID, channelID, static_cast<int32>(flag), value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_setChannelVariableAsString(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, const FString& value, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_setChannelVariableAsString(serverConnectionHandlerID, channelID, static_cast<int32>(flag), value);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_flushChannelUpdates(int32 serverConnectionHandlerID, int32 channelID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_flushChannelUpdates(serverConnectionHandlerID, channelID, returnCode);
    error = handler.getErrorCode();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_flushChannelCreation(int32 serverConnectionHandlerID, int32 channelParentID, const FString& returnCode, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_flushChannelCreation(serverConnectionHandlerID, channelParentID, returnCode);
    error = handler.getErrorCode();
}
TArray<int32> UTeamSpeakFunctionLibrary::TeamSpeak_getChannelList(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelList(serverConnectionHandlerID);
    error = handler.getErrorCode();
	return handler.getValue();
}
TArray<int32> UTeamSpeakFunctionLibrary::TeamSpeak_getChannelClientList(int32 serverConnectionHandlerID, int32 channelID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelClientList(serverConnectionHandlerID, channelID);
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getParentChannelOfChannel(int32 serverConnectionHandlerID, int32 channelID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getParentChannelOfChannel(serverConnectionHandlerID, channelID);
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getChannelEmptySecs(int32 serverConnectionHandlerID, int32 channelID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getChannelEmptySecs(serverConnectionHandlerID, channelID);
    error = handler.getErrorCode();
	return handler.getValue();
}
TArray<int32> UTeamSpeakFunctionLibrary::TeamSpeak_getServerConnectionHandlerList(int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getServerConnectionHandlerList();
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getServerVariableAsInt(int32 serverConnectionHandlerID, ETeamSpeak_VirtualServerProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getServerVariableAsInt(serverConnectionHandlerID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
int32 UTeamSpeakFunctionLibrary::TeamSpeak_getServerVariableAsUint64(int32 serverConnectionHandlerID, ETeamSpeak_VirtualServerProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getServerConnectionVariableAsUInt64(serverConnectionHandlerID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
FString UTeamSpeakFunctionLibrary::TeamSpeak_getServerVariableAsString(int32 serverConnectionHandlerID, ETeamSpeak_VirtualServerProperties flag, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_getServerVariableAsString(serverConnectionHandlerID, static_cast<int32>(flag));
    error = handler.getErrorCode();
	return handler.getValue();
}
void UTeamSpeakFunctionLibrary::TeamSpeak_requestServerVariables(int32 serverConnectionHandlerID, int32 &error) {
	auto handler = ts_Manager->UE_TS3_SDK_requestServerVariables(serverConnectionHandlerID);
    error = handler.getErrorCode();
}

void UTeamSpeakFunctionLibrary::TeamSpeak_break_all_bindings()
{
	auto&& ts_fl = UTeamSpeakFunctionLibrary::getUTeamSpeakFunctionLibrary();

	ts_fl->onConnectStatusChangeEvent.Clear();
	ts_fl->onServerProtocolVersionEvent.Clear();
	ts_fl->onNewChannelEvent.Clear();
	ts_fl->onNewChannelCreatedEvent.Clear();
	ts_fl->onDelChannelEvent.Clear();
	ts_fl->onChannelMoveEvent.Clear();
	ts_fl->onUpdateChannelEvent.Clear();
	ts_fl->onUpdateChannelEditedEvent.Clear();
	ts_fl->onUpdateClientEvent.Clear();
	ts_fl->onClientMoveEvent.Clear();
	ts_fl->onClientMoveSubscriptionEvent.Clear();
	ts_fl->onClientMoveTimeoutEvent.Clear();
	ts_fl->onClientMoveMovedEvent.Clear();
	ts_fl->onClientKickFromChannelEvent.Clear();
	ts_fl->onClientKickFromServerEvent.Clear();
	ts_fl->onClientIDsEvent.Clear();
	ts_fl->onClientIDsFinishedEvent.Clear();
	ts_fl->onServerEditedEvent.Clear();
	ts_fl->onServerUpdatedEvent.Clear();
	ts_fl->onServerErrorEvent.Clear();
	ts_fl->onServerStopEvent.Clear();
	ts_fl->onTextMessageEvent.Clear();
	ts_fl->onTalkStatusChangeEvent.Clear();
	ts_fl->onIgnoredWhisperEvent.Clear();
	ts_fl->onConnectionInfoEvent.Clear();
	ts_fl->onServerConnectionInfoEvent.Clear();
	ts_fl->onChannelSubscribeEvent.Clear();
	ts_fl->onChannelSubscribeFinishedEvent.Clear();
	ts_fl->onChannelUnsubscribeEvent.Clear();
	ts_fl->onChannelUnsubscribeFinishedEvent.Clear();
	ts_fl->onChannelDescriptionUpdateEvent.Clear();
	ts_fl->onChannelPasswordChangedEvent.Clear();
	ts_fl->onPlaybackShutdownCompleteEvent.Clear();
	ts_fl->onSoundDeviceListChangedEvent.Clear();
	ts_fl->onUserLoggingMessageEvent.Clear();
	ts_fl->onProvisioningSlotRequestResultEvent.Clear();
	ts_fl->onCheckServerUniqueIdentifierEvent.Clear();
	ts_fl->onClientPasswordEncrypt.Clear();
}