#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

#include "TeamSpeakFunctionLibrary.generated.h"

class TeamSpeak_Manager;

UENUM(BlueprintType)
enum class ETeamSpeak_Visibility : uint8 {
	ENTER = 0,
	RETAIN,
	LEAVE
};

UENUM(BlueprintType)
enum class ETeamSpeak_TalkStatus : uint8 {
	STATUS_NOT_TALKING = 0,
	STATUS_TALKING = 1,
	STATUS_TALKING_WHILE_DISABLED = 2,
};

UENUM(BlueprintType)
enum class  ETeamSpeak_CodecType : uint8 {
	CODEC_SPEEX_NARROWBAND = 0,   //mono,   16bit,  8kHz, bitrate dependent on the quality setting
	CODEC_SPEEX_WIDEBAND,         //mono,   16bit, 16kHz, bitrate dependent on the quality setting
	CODEC_SPEEX_ULTRAWIDEBAND,    //mono,   16bit, 32kHz, bitrate dependent on the quality setting
	CODEC_CELT_MONO,              //mono,   16bit, 48kHz, bitrate dependent on the quality setting
	CODEC_OPUS_VOICE,             //mono,   16bit, 48khz, bitrate dependent on the quality setting, optimized for voice
	CODEC_OPUS_MUSIC,             //stereo, 16bit, 48khz, bitrate dependent on the quality setting, optimized for music
};

UENUM(BlueprintType)
enum class  ETeamSpeak_CodecEncryptionMode : uint8 {
	CODEC_ENCRYPTION_PER_CHANNEL = 0,
	CODEC_ENCRYPTION_FORCED_OFF,
	CODEC_ENCRYPTION_FORCED_ON,
};

UENUM(BlueprintType)
enum class ETeamSpeak_TextMessageTargetMode : uint8 {
	TextMessageTarget_INVALID = 0,
	TextMessageTarget_CLIENT,
	TextMessageTarget_CHANNEL,
	TextMessageTarget_SERVER,
	TextMessageTarget_MAX
};

UENUM(BlueprintType)
enum class ETeamSpeak_MuteInputStatus : uint8 {
	MUTEINPUT_NONE = 0,
	MUTEINPUT_MUTED,
};

UENUM(BlueprintType)
enum class ETeamSpeak_MuteOutputStatus : uint8 {
	MUTEOUTPUT_NONE = 0,
	MUTEOUTPUT_MUTED,
};

UENUM(BlueprintType)
enum class ETeamSpeak_HardwareInputStatus : uint8 {
	HARDWAREINPUT_DISABLED = 0,
	HARDWAREINPUT_ENABLED,
};

UENUM(BlueprintType)
enum class ETeamSpeak_HardwareOutputStatus : uint8 {
	HARDWAREOUTPUT_DISABLED = 0,
	HARDWAREOUTPUT_ENABLED,
};

UENUM(BlueprintType)
enum class ETeamSpeak_InputDeactivationStatus : uint8 {
	INPUT_ACTIVE = 0,
	INPUT_DEACTIVATED = 1,
};

UENUM(BlueprintType)
enum class ETeamSpeak_ReasonIdentifier : uint8 {
	REASON_NONE = 0,  //no reason data
	REASON_MOVED = 1,  //{SectionInvoker}
	REASON_SUBSCRIPTION = 2,  //no reason data
	REASON_LOST_CONNECTION = 3,  //reasonmsg=reason
	REASON_KICK_CHANNEL = 4,  //{SectionInvoker} reasonmsg=reason               //{SectionInvoker} is only added server->client
	REASON_KICK_SERVER = 5,  //{SectionInvoker} reasonmsg=reason               //{SectionInvoker} is only added server->client
	REASON_KICK_SERVER_BAN = 6,  //{SectionInvoker} reasonmsg=reason bantime=time  //{SectionInvoker} is only added server->client
	REASON_SERVERSTOP = 7,  //reasonmsg=reason
	REASON_CLIENTDISCONNECT = 8,  //reasonmsg=reason
	REASON_CHANNELUPDATE = 9,  //no reason data
	REASON_CHANNELEDIT = 10, //{SectionInvoker}
	REASON_CLIENTDISCONNECT_SERVER_SHUTDOWN = 11,  //reasonmsg=reason
};

UENUM(BlueprintType)
enum class ETeamSpeak_ChannelProperties : uint8 {
	CHANNEL_NAME = 0,                       //Available for all channels that are "in view", always up-to-date
	CHANNEL_TOPIC,                          //Available for all channels that are "in view", always up-to-date
	CHANNEL_DESCRIPTION,                    //Must be requested (=> requestChannelDescription)
	CHANNEL_PASSWORD,                       //not available client side
	CHANNEL_CODEC,                          //Available for all channels that are "in view", always up-to-date
	CHANNEL_CODEC_QUALITY,                  //Available for all channels that are "in view", always up-to-date
	CHANNEL_MAXCLIENTS,                     //Available for all channels that are "in view", always up-to-date
	CHANNEL_MAXFAMILYCLIENTS,               //Available for all channels that are "in view", always up-to-date
	CHANNEL_ORDER,                          //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_PERMANENT,                 //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_SEMI_PERMANENT,            //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_DEFAULT,                   //Available for all channels that are "in view", always up-to-date
	CHANNEL_FLAG_PASSWORD,                  //Available for all channels that are "in view", always up-to-date
	CHANNEL_CODEC_LATENCY_FACTOR,           //Available for all channels that are "in view", always up-to-date
	CHANNEL_CODEC_IS_UNENCRYPTED,           //Available for all channels that are "in view", always up-to-date
	CHANNEL_SECURITY_SALT,                  //Not available client side, not used in teamspeak, only SDK. Sets the options+salt for security hash.
	CHANNEL_DELETE_DELAY,                   //How many seconds to wait before deleting this channel
	CHANNEL_ENDMARKER,
};

UENUM(BlueprintType)
enum class ETeamSpeak_ClientProperties : uint8 {
	CLIENT_UNIQUE_IDENTIFIER = 0,           //automatically up-to-date for any client "in view", can be used to identify this particular client installation
	CLIENT_NICKNAME,                        //automatically up-to-date for any client "in view"
	CLIENT_VERSION,                         //for other clients than ourself, this needs to be requested (=> requestClientVariables)
	CLIENT_PLATFORM,                        //for other clients than ourself, this needs to be requested (=> requestClientVariables)
	CLIENT_FLAG_TALKING,                    //automatically up-to-date for any client that can be heard (in room / whisper)
	CLIENT_INPUT_MUTED,                     //automatically up-to-date for any client "in view", this clients microphone mute status
	CLIENT_OUTPUT_MUTED,                    //automatically up-to-date for any client "in view", this clients headphones/speakers/mic combined mute status
	CLIENT_OUTPUTONLY_MUTED,                //automatically up-to-date for any client "in view", this clients headphones/speakers only mute status
	CLIENT_INPUT_HARDWARE,                  //automatically up-to-date for any client "in view", this clients microphone hardware status (is the capture device opened?)
	CLIENT_OUTPUT_HARDWARE,                 //automatically up-to-date for any client "in view", this clients headphone/speakers hardware status (is the playback device opened?)
	CLIENT_INPUT_DEACTIVATED,               //only usable for ourself, not propagated to the network
	CLIENT_IDLE_TIME,                       //internal use
	CLIENT_DEFAULT_CHANNEL,                 //only usable for ourself, the default channel we used to connect on our last connection attempt
	CLIENT_DEFAULT_CHANNEL_PASSWORD,        //internal use
	CLIENT_SERVER_PASSWORD,                 //internal use
	CLIENT_META_DATA,                       //automatically up-to-date for any client "in view", not used by TeamSpeak, free storage for sdk users
	CLIENT_IS_MUTED,                        //only make sense on the client side locally, "1" if this client is currently muted by us, "0" if he is not
	CLIENT_IS_RECORDING,                    //automatically up-to-date for any client "in view"
	CLIENT_VOLUME_MODIFICATOR,              //internal use
	CLIENT_VERSION_SIGN,					//sign
	CLIENT_SECURITY_HASH,                   //SDK use, not used by teamspeak. Hash is provided by an outside source. A channel will use the security salt + other client data to calculate a hash, which must be the same as the one provided here.
	CLIENT_ENDMARKER,
};

UENUM(BlueprintType)
enum class ETeamSpeak_VirtualServerProperties : uint8 {
	VIRTUALSERVER_UNIQUE_IDENTIFIER = 0,             //available when connected, can be used to identify this particular server installation
	VIRTUALSERVER_NAME,                              //available and always up-to-date when connected
	VIRTUALSERVER_WELCOMEMESSAGE,                    //available when connected,  (=> requestServerVariables)
	VIRTUALSERVER_PLATFORM,                          //available when connected
	VIRTUALSERVER_VERSION,                           //available when connected
	VIRTUALSERVER_MAXCLIENTS,                        //only available on request (=> requestServerVariables), stores the maximum number of clients that may currently join the server
	VIRTUALSERVER_PASSWORD,                          //not available to clients, the server password
	VIRTUALSERVER_CLIENTS_ONLINE,                    //only available on request (=> requestServerVariables),
	VIRTUALSERVER_CHANNELS_ONLINE,                   //only available on request (=> requestServerVariables),
	VIRTUALSERVER_CREATED,                           //available when connected, stores the time when the server was created
	VIRTUALSERVER_UPTIME,                            //only available on request (=> requestServerVariables), the time since the server was started
	VIRTUALSERVER_CODEC_ENCRYPTION_MODE,             //available and always up-to-date when connected
	VIRTUALSERVER_ENDMARKER,
};

UENUM(BlueprintType)
enum class ETeamSpeak_LogTypes : uint8 {
	LogType_NONE = 0x0000,
	LogType_FILE = 0x0001,
	LogType_CONSOLE = 0x0002,
	LogType_USERLOGGING = 0x0004,
	LogType_NO_NETLOGGING = 0x0008,
	LogType_DATABASE = 0x0010,
    LogType_SYSLOG = 0x0020,
};

UENUM(BlueprintType)
enum class ETeamSpeak_LogLevel : uint8 {
	LogLevel_CRITICAL = 0, //these messages stop the program
	LogLevel_ERROR,        //everything that is really bad, but not so bad we need to shut down
	LogLevel_WARNING,      //everything that *might* be bad
	LogLevel_DEBUG,        //output that might help find a problem
	LogLevel_INFO,         //informational output, like "starting database version x.y.z"
	LogLevel_DEVEL         //developer only output (will not be displayed in release mode)
};

UENUM(BlueprintType)
enum class ETeamSpeak_GroupWhisperType : uint8 {
	GROUPWHISPERTYPE_SERVERGROUP = 0,
	GROUPWHISPERTYPE_CHANNELGROUP = 1,
	GROUPWHISPERTYPE_CHANNELCOMMANDER = 2,
	GROUPWHISPERTYPE_ALLCLIENTS = 3,
	GROUPWHISPERTYPE_ENDMARKER,
};

UENUM(BlueprintType)
enum class ETeamSpeak_GroupWhisperTargetMode : uint8 {
	GROUPWHISPERTARGETMODE_ALL = 0,
	GROUPWHISPERTARGETMODE_CURRENTCHANNEL = 1,
	GROUPWHISPERTARGETMODE_PARENTCHANNEL = 2,
	GROUPWHISPERTARGETMODE_ALLPARENTCHANNELS = 3,
	GROUPWHISPERTARGETMODE_CHANNELFAMILY = 4,
	GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY = 5,
	GROUPWHISPERTARGETMODE_SUBCHANNELS = 6,
	GROUPWHISPERTARGETMODE_ENDMARKER,
};

UENUM(BlueprintType)
enum class ETeamSpeak_MonoSoundDestination : uint8 {
	MONO_SOUND_DESTINATION_ALL = 0, /* Send mono sound to all available speakers */
	MONO_SOUND_DESTINATION_FRONT_CENTER = 1, /* Send mono sound to front center speaker if available */
	MONO_SOUND_DESTINATION_FRONT_LEFT_AND_RIGHT = 2  /* Send mono sound to front left/right speakers if available */
};

UENUM(BlueprintType)
enum class ETeamSpeak_SecuritySaltOptions : uint8 {
	SECURITY_SALT_CHECK_INVALID = 0,
	SECURITY_SALT_CHECK_NICKNAME = 1, /* put nickname into security hash */
	SECURITY_SALT_CHECK_META_DATA = 2  /* put (game)meta data into security hash */
};

UENUM(BlueprintType)
enum class ETeamSpeak_ClientCommand : uint8 {
	CLIENT_COMMAND_requestConnectionInfo = 0,
	CLIENT_COMMAND_requestClientMove = 1,
	CLIENT_COMMAND_requestXXMuteClients = 2,
	CLIENT_COMMAND_requestClientKickFromXXX = 3,
	CLIENT_COMMAND_flushChannelCreation = 4,
	CLIENT_COMMAND_flushChannelUpdates = 5,
	CLIENT_COMMAND_requestChannelMove = 6,
	CLIENT_COMMAND_requestChannelDelete = 7,
	CLIENT_COMMAND_requestChannelDescription = 8,
	CLIENT_COMMAND_requestChannelXXSubscribeXXX = 9,
	CLIENT_COMMAND_requestServerConnectionInfo = 10,
	CLIENT_COMMAND_requestSendXXXTextMsg = 11,
	CLIENT_COMMAND_ENDMARKER = 12
};

UENUM(BlueprintType)
enum class ETeamSpeak_ACLType : uint8 {
	ACL_NONE = 0,
	ACL_WHITE_LIST = 1,
	ACL_BLACK_LIST = 2
};

UENUM(BlueprintType)
enum class EConnectStatus : uint8 {
    STATUS_DISCONNECTED = 0,       //There is no activity to the server, this is the default value
    STATUS_CONNECTING = 1,             //We are trying to connect, we haven't got a clientID yet, we haven't been accepted by the server
    STATUS_CONNECTED = 2,              //The server has accepted us, we can talk and hear and we got a clientID, but we don't have the channels and clients yet, we can get server infos (welcome msg etc.)
    STATUS_CONNECTION_ESTABLISHING = 3,//we are CONNECTED and we are visible
    STATUS_CONNECTION_ESTABLISHED = 4, //we are CONNECTED and we have the client and channels available
};

USTRUCT(BlueprintType)
struct FTeamSpeak_2dArray {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> value;

	FTeamSpeak_2dArray(const TArray<FString>& v) {
		value = v;
	}

    FTeamSpeak_2dArray() = default;
};

UCLASS()
class TEAMSPEAK_SDK_API UTeamSpeakFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Statics")
	static UTeamSpeakFunctionLibrary* getUTeamSpeakFunctionLibrary();
	
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Debug")
	static void printMessageDebug(FString str);

	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Debug")
	static FString getSoundBackendDir_Editor();

	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static void TeamSpeak_initClientLib(ETeamSpeak_LogTypes usedLogTypes, const FString& logFileFolder, const FString& resourcesFolder, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static void TeamSpeak_destroyClientLib(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static FString TeamSpeak_getClientLibVersion(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static int32 TeamSpeak_getClientLibVersionNumber(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static int32 TeamSpeak_spawnNewServerConnectionHandler(int32 port, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static void TeamSpeak_destroyServerConnectionHandler(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static FString TeamSpeak_createIdentity(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static FString TeamSpeak_identityStringToUniqueIdentifier(const FString& identityString, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static TArray<FTeamSpeak_2dArray> TeamSpeak_getPlaybackDeviceList(const FString& modeID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static TArray<FTeamSpeak_2dArray> TeamSpeak_getCaptureDeviceList(const FString& modeID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static TArray<FString> TeamSpeak_getPlaybackModeList(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static TArray<FString> TeamSpeak_getCaptureModeList(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static TArray<FString> TeamSpeak_getDefaultPlaybackDevice(const FString& modeID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static TArray<FString> TeamSpeak_getDefaultCaptureDevice(const FString& modeID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getDefaultPlayBackMode(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getDefaultCaptureMode(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_openPlaybackDevice(int32 serverConnectionHandlerID, const FString& modeID, const FString& playbackDevice, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_openCaptureDevice(int32 serverConnectionHandlerID, const FString& modeID, const FString& captureDevice, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getCurrentPlaybackDeviceName(int32 serverConnectionHandlerID, int32 &error, int32 &isDefault);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getCurrentPlayBackMode(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getCurrentCaptureDeviceName(int32 serverConnectionHandlerID, int32 &error, int32 &isDefault);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getCurrentCaptureMode(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_initiateGracefulPlaybackShutdown(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_closePlaybackDevice(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_closeCaptureDevice(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_activateCaptureDevice(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_playWaveFile(int32 serverConnectionHandlerID, const FString& path, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_playWaveFileHandle(int32 serverConnectionHandlerID, const FString& path, int32 loop, int32 waveHandle, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_pauseWaveFileHandle(int32 serverConnectionHandlerID, int32 waveHandle, int32 pause, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_closeWaveFileHandle(int32 serverConnectionHandlerID, int32 waveHandle, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_registerCustomDevice(const FString& deviceID, const FString& deviceDisplayName, int32 capFrequency, int32 capChannels, int32 playFrequency, int32 playChannels, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_unregisterCustomDevice(const FString& deviceID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_processCustomCaptureData(const FString& deviceName, const TArray<int32>& buffer, int32 samples, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_acquireCustomPlaybackData(const FString& deviceName, TArray<int32>& buffer, int32 samples, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_setLocalTestMode(int32 serverConnectionHandlerID, int32 status, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_startVoiceRecording(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_stopVoiceRecording(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Whisper")
	static void TeamSpeak_allowWhispersFrom(int32 serverConnectionHandlerID, int32 clID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Whisper")
	static void TeamSpeak_removeFromAllowedWhispersFrom(int32 serverConnectionHandlerID, int32 clID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_systemset3DListenerAttributes(int32 serverConnectionHandlerID, FVector position, FVector forward, FVector up, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_set3DWaveAttributes(int32 serverConnectionHandlerID, int32 waveHandle, FVector position, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_systemset3DSettings(int32 serverConnectionHandlerID, float distanceFactor, float rolloffScale, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_channelset3DAttributes(int32 serverConnectionHandlerID, int32 clientID, FVector position, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static float TeamSpeak_getPreProcessorInfoValueFloat(int32 serverConnectionHandlerID, const FString& ident, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getPreProcessorConfigValue(int32 serverConnectionHandlerID, const FString& ident, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_setPreProcessorConfigValue(int32 serverConnectionHandlerID, const FString& ident, const FString& value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static FString TeamSpeak_getEncodeConfigValu(int32 serverConnectionHandlerID, const FString& ident, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static float TeamSpeak_getPlaybackConfigValueAsFloat(int32 serverConnectionHandlerID, const FString& ident, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_setPlaybackConfigValue(int32 serverConnectionHandlerID, const FString& ident, const FString& value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Sound")
	static void TeamSpeak_setClientVolumeModifier(int32 serverConnectionHandlerID, int32 clientID, float value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Log")
	static void TeamSpeak_logMessage(const FString& logMessage, int32 severity, const FString& channel, int32 logID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Log")
	static void TeamSpeak_setLogVerbosity(int32 logVerbosity, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|ErrorHandling")
	static FString TeamSpeak_getErrorMessage(int32 errorCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static void TeamSpeak_startConnection(int32 serverConnectionHandlerID, const FString& identity, const FString& ip, int32 port, const FString& nickname, const TArray<FString>& defaultChannelArray, const FString& defaultChannelPassword, const FString& serverPassword, int32 &error);
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
    static void TeamSpeak_startConnectionWithChannelID(int32 serverConnectionHandlerID, const FString& identity, const FString& ip, int32 port, const FString& nickname, int32 defaultChannelId, const FString& defaultChannelPassword, const FString& serverPassword, int32& error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static void TeamSpeak_stopConnection(int32 serverConnectionHandlerID, const FString& quitMessage, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestClientMove(int32 serverConnectionHandlerID, int32 clientID, int32 newChannelID, const FString& password, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestClientVariables(int32 serverConnectionHandlerID, int32 clientID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestClientKickFromChannel(int32 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestClientKickFromServer(int32 serverConnectionHandlerID, int32 clientID, const FString& kickReason, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_requestChannelDelete(int32 serverConnectionHandlerID, int32 channelID, int32 force, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_requestChannelMove(int32 serverConnectionHandlerID, int32 channelID, int32 newChannelParentID, int32 newChannelOrder, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Message")
	static void TeamSpeak_requestSendPrivateTextMsg(int32 serverConnectionHandlerID, const FString& message, int32 targetClientID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Message")
	static void TeamSpeak_requestSendChannelTextMsg(int32 serverConnectionHandlerID, const FString& message, int32 targetChannelID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Message")
	static void TeamSpeak_requestSendServerTextMsg(int32 serverConnectionHandlerID, const FString& message, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static void TeamSpeak_requestConnectionInfo(int32 serverConnectionHandlerID, int32 clientID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Whisper")
	static void TeamSpeak_requestClientSetWhisperList(int32 serverConnectionHandlerID, int32 clientID, const TArray<int32>& targetChannelIDArray, const TArray<int32>& targetClientIDArray, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Subscription")
	static void TeamSpeak_requestChannelSubscribe(int32 serverConnectionHandlerID, const TArray<int32>& channelIDArray, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Subscription")
	static void TeamSpeak_requestChannelSubscribeAll(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Subscription")
	static void TeamSpeak_requestChannelUnsubscribe(int32 serverConnectionHandlerID, const TArray<int32>& channelIDArray, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Subscription")
	static void TeamSpeak_requestChannelUnsubscribeAll(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_requestChannelDescription(int32 serverConnectionHandlerID, int32 channelID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestMuteClients(int32 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestUnmuteClients(int32 serverConnectionHandlerID, const TArray<int32>& clientIDArray, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_requestClientIDs(int32 serverConnectionHandlerID, const FString& clientUniqueIdentifier, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Provisioning")
	static int32 TeamSpeak_requestSlotsFromProvisioningServer(const FString& ip, int32 port, const FString& serverPassword, int32 slots, const FString& identity, const FString& region, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Provisioning")
	static void TeamSpeak_cancelRequestSlotsFromProvisioningServer(int32 requestHandle, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Provisioning")
	static void TeamSpeak_startConnectionWithProvisioningKey(int32 serverConnectionHandlerID, const FString& identity, const FString& nickname, const FString& connectionKey, const FString& clientMetaData, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static int32 TeamSpeak_getClientID(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static int32 TeamSpeak_getConnectionStatus(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static void TeamSpeak_cleanUpConnectionInfo(int32 serverConnectionHandlerID, int32 clientID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static void TeamSpeak_requestServerConnectionInfo(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static int32 TeamSpeak_getClientSelfVariableAsint32(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static FString TeamSpeak_getClientSelfVariableAsString(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_setClientSelfVariableAsint32(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, int32 value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_setClientSelfVariableAsString(int32 serverConnectionHandlerID, ETeamSpeak_ClientProperties flag, const FString& value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static void TeamSpeak_flushClientSelfUpdates(int32 serverConnectionHandlerID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static int32 TeamSpeak_getClientVariableAsInt(int32 serverConnectionHandlerID, int32 clientID, ETeamSpeak_ClientProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static int32 TeamSpeak_getClientVariableAsUint64(int32 serverConnectionHandlerID, int32 clientID, ETeamSpeak_ClientProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static FString TeamSpeak_getClientVariableAsString(int32 serverConnectionHandlerID, int32 clientID, ETeamSpeak_ClientProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Client")
	static TArray<int32> TeamSpeak_getClientList(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static int32 TeamSpeak_getChannelOfClient(int32 serverConnectionHandlerID, int32 clientID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static int32 TeamSpeak_getChannelVariableAsInt(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static int32 TeamSpeak_getChannelVariableAsUint64(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static FString TeamSpeak_getChannelVariableAsString(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static int32 TeamSpeak_getChannelIDFromChannelNames(int32 serverConnectionHandlerID, const TArray<FString>& channelNameArray, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_setChannelVariableAsInt(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_setChannelVariableAsUint64(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, int32 value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_setChannelVariableAsString(int32 serverConnectionHandlerID, int32 channelID, ETeamSpeak_ChannelProperties flag, const FString& value, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_flushChannelUpdates(int32 serverConnectionHandlerID, int32 channelID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static void TeamSpeak_flushChannelCreation(int32 serverConnectionHandlerID, int32 channelParentID, const FString& returnCode, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static TArray<int32> TeamSpeak_getChannelList(int32 serverConnectionHandlerID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static TArray<int32> TeamSpeak_getChannelClientList(int32 serverConnectionHandlerID, int32 channelID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static int32 TeamSpeak_getParentChannelOfChannel(int32 serverConnectionHandlerID, int32 channelID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Channel")
	static int32 TeamSpeak_getChannelEmptySecs(int32 serverConnectionHandlerID, int32 channelID, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static TArray<int32> TeamSpeak_getServerConnectionHandlerList(int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static int32 TeamSpeak_getServerVariableAsUint64(int32 serverConnectionHandlerID, ETeamSpeak_VirtualServerProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static int32 TeamSpeak_getServerVariableAsInt(int32 serverConnectionHandlerID, ETeamSpeak_VirtualServerProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static FString TeamSpeak_getServerVariableAsString(int32 serverConnectionHandlerID, ETeamSpeak_VirtualServerProperties flag, int32 &error);
	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Connection")
	static void TeamSpeak_requestServerVariables(int32 serverConnectionHandlerID, int32 &error);

	UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Function|Init/Destroy")
	static void TeamSpeak_break_all_bindings();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamSpeakOnConnectStatusChangeEvent, int32, serverconnectionHandlerID, EConnectStatus, newStatus, int32, errorNumber);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Connection")
	FTeamSpeakOnConnectStatusChangeEvent onConnectStatusChangeEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnServerProtocolVersionEvent, int32, serverconnectionHandlerID, int32, protocolVersion);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Server")
	FTeamSpeakOnServerProtocolVersionEvent onServerProtocolVersionEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamSpeakOnNewChannelEvent, int32, serverconnectionHandlerID, int32, channelID, int32, channelParentID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnNewChannelEvent onNewChannelEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FTeamSpeakOnNewChannelCreatedEvent, int32, serverconnectionHandlerID, int32, channelID, int32, channelParentID, int32, invokerID, const FString&, invokerName, const FString&, invokerUniqueIdentifier);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnNewChannelCreatedEvent onNewChannelCreatedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTeamSpeakOnDelChannelEvent, int32, serverconnectionHandlerID, int32, channelID, int32, invokerID, const FString&, invokerName, const FString&, invokerUniqueIdentifier);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnDelChannelEvent onDelChannelEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FTeamSpeakOnChannelMoveEvent, int32, serverconnectionHandlerID, int32, channelID, int32, newChannelParentID, int32, invokerID, const FString&, invokerName, const FString&, invokerUniqueIdentifier);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelMoveEvent onChannelMoveEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnUpdateChannelEvent, int32, serverconnectionHandlerID, int32, channelID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnUpdateChannelEvent onUpdateChannelEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTeamSpeakOnUpdateChannelEditedEvent, int32, serverconnectionHandlerID, int32, channelID, int32,  invokerID, const FString&, invokerName, const FString&, invokerUniqueIdentifier);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnUpdateChannelEditedEvent onUpdateChannelEditedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTeamSpeakOnUpdateClientEvent, int32, serverConnectionHalderID, int32, clientID, int32, invokerID, const FString&, invokerName, const FString&, invokerUniqueIndentivier);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnUpdateClientEvent onUpdateClientEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FTeamSpeakOnClientMoveEvent, int32, serverconnectionHandlerID, int32, clientID, int32, oldChannelID, int32, newChannelID, ETeamSpeak_Visibility, visibility, const FString&, moveMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientMoveEvent onClientMoveEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTeamSpeakOnClientMoveSubscriptionEvent, int32, serverconnectionHandlerID, int32, clientID, int32, oldChannelID, int32, newChannelID, ETeamSpeak_Visibility, visibility);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientMoveSubscriptionEvent onClientMoveSubscriptionEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FTeamSpeakOnClientMoveTimeoutEvent, int32, serverconnectionHandlerID, int32, clientID, int32, oldChannelID, int32, newChannelID, ETeamSpeak_Visibility, visibility, const FString&, moveMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientMoveTimeoutEvent onClientMoveTimeoutEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FTeamSpeakOnClientMoveMovedEvent, int32, serverconnectionHandlerID, int32, clientID, int32, oldChannelID, int32, newChannelID, ETeamSpeak_Visibility, visibility, int32, moverID, const TArray<FString>&, moverNameAndUniqueIdentifier, const FString&, moveMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientMoveMovedEvent onClientMoveMovedEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FTeamSpeakOnClientKickFromChannelEvent, int32, serverconnectionHandlerID, int32, clientID, int32, oldChannelID, int32, newChannelID, ETeamSpeak_Visibility, visibility, int32, kickerID, const TArray<FString>&, moverNameAndUniqueIdentifier, const FString&, kickMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientKickFromChannelEvent onClientKickFromChannelEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FTeamSpeakOnClientKickFromServerEvent, int32, serverconnectionHandlerID, int32, clientID, int32, oldChannelID, int32, newChannelID, ETeamSpeak_Visibility, visibility, int32, kickerID, const TArray<FString>&, moverNameAndUniqueIdentifier, const FString&, kickMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientKickFromServerEvent onClientKickFromServerEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTeamSpeakOnClientIDsEvent, int32, serverconnectionHandlerID, const FString&, uniqueClientIdentifier, int32, clientID, const FString&, clientName);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientIDsEvent onClientIDsEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSpeakOnClientIDsFinishedEvent, int32, serverconnectionHandlerID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Client")
	FTeamSpeakOnClientIDsFinishedEvent onClientIDsFinishedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTeamSpeakOnServerEditedEvent, int32, serverconnectionHandlerID, int32, editerID, const FString&, editerName, const FString&, editerUniqueIdentifier);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Server")
	FTeamSpeakOnServerEditedEvent onServerEditedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSpeakOnServerUpdatedEvent, int32, serverconnectionHandlerID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Server")
	FTeamSpeakOnServerUpdatedEvent onServerUpdatedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTeamSpeakOnServerErrorEvent, int32, serverConnectionHandlerID, const FString&, errorMessage, int32, error, const FString&, returnCode, const FString&, extraMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Server")
	FTeamSpeakOnServerErrorEvent onServerErrorEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnServerStopEvent, int32, serverconnectionHandlerID, const FString&, shutdownMessage);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Server")
	FTeamSpeakOnServerStopEvent onServerStopEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FTeamSpeakOnTextMessageEvent, int32, serverconnectionHandlerID, int32, targetMode, int32, toID, int32, fromID, const FString&, fromName, const FString&, fromUniqueIdentifier, const FString&, message);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Messaging")
	FTeamSpeakOnTextMessageEvent onTextMessageEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTeamSpeakOnTalkStatusChangedEvent, int32, serverConnectionHandlerID, int32, status, int32, isReceivedWhisper, int32, clientID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Talk")
	FTeamSpeakOnTalkStatusChangedEvent onTalkStatusChangeEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnIgnoredWhisperEvent, int32, serverconnectionHandlerID, int32, clientID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Talk")
	FTeamSpeakOnIgnoredWhisperEvent onIgnoredWhisperEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnConnetrionInfoEvent, int32, serverconnectionHandlerID, int32, clientID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Connection")
	FTeamSpeakOnConnetrionInfoEvent onConnectionInfoEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSpeakOnServerConnectionInfoEvent, int32, serverconnectionHandlerID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Connection")
	FTeamSpeakOnServerConnectionInfoEvent onServerConnectionInfoEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnChannelSubscribeEvent, int32, serverconnectionHandlerID, int32, channelID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelSubscribeEvent onChannelSubscribeEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSpeakOnChannelSubscribeFinishedEvent, int32, serverconnectionHandlerID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelSubscribeFinishedEvent onChannelSubscribeFinishedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnChannelUnsubscribeEvent, int32, serverconnectionHandlerID, int32, channelID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelUnsubscribeEvent onChannelUnsubscribeEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSpeakOnChannelUnsubscribeFinishedEvent, int32, serverconnectionHandlerID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelUnsubscribeFinishedEvent onChannelUnsubscribeFinishedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnChannelDescriptionUpdateEvent, int32, serverconnectionHandlerID, int32, channelID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelDescriptionUpdateEvent onChannelDescriptionUpdateEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnChannelPasswordChangedEvent, int32, serverconnectionHandlerID, int32, channelID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Channel")
	FTeamSpeakOnChannelPasswordChangedEvent onChannelPasswordChangedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSpeakOnPlaybackShutdownCompleteEvent, int32, serverconnectionHandlerID);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Device")
	FTeamSpeakOnPlaybackShutdownCompleteEvent onPlaybackShutdownCompleteEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamSpeakOnSoundDeviceListChangeEvent, const FString&, modeID, int32, playOrCap);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Device")
	FTeamSpeakOnSoundDeviceListChangeEvent onSoundDeviceListChangedEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FTeamSpeakOnUserLoggingMessageEvent, const FString&, logMessage, int32, logLevel, const FString&, logChannel, int32, logID, const FString&, logTime, const FString&, completeLogString);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Message")
	FTeamSpeakOnUserLoggingMessageEvent onUserLoggingMessageEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamSpeakOnProvisioningSlotRequestedResultEvent, int32, error, int32, requestHandle, const FString&, connectionKey);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Connection")
	FTeamSpeakOnProvisioningSlotRequestedResultEvent onProvisioningSlotRequestResultEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamSpeakOnCheckServerUniqueIdentifierEvent, int32, serverConnectionHandlerID, const FString&, serverUniqueidentifier, const TArray<int32>&, cancelConnect);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Server")
	FTeamSpeakOnCheckServerUniqueIdentifierEvent onCheckServerUniqueIdentifierEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTeamSpeakOnClientPasswordEncrypt, int32, serverconnectionHandlerID, const FString&, plaint32ext, const FString&, encryptedText, int32, encryptedTextByteSize);
	UPROPERTY(BlueprintAssignable, Category = "TeamSpeak|Callbacks|Crypt")
	FTeamSpeakOnClientPasswordEncrypt onClientPasswordEncrypt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_TalkStatus TalkStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_CodecType CodecType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_CodecEncryptionMode CodecEncryptionMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_TextMessageTargetMode TextMessageTargetMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_MuteInputStatus MuteInputStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_MuteOutputStatus MuteOutputStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_HardwareInputStatus HardwareInputStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_HardwareOutputStatus HardwareOutputStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_InputDeactivationStatus InputDeactivationstatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_ReasonIdentifier ReasonIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_ChannelProperties ChannelProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_ClientProperties ClientProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_VirtualServerProperties VirtualServerProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_LogTypes LogTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_LogLevel LogLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_GroupWhisperType GroupWhisperType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_GroupWhisperTargetMode GroupWhisperTargetMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_MonoSoundDestination MonoSoundDestination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_SecuritySaltOptions SecuritySaltOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_ClientCommand ClientCommand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamSpeak|Enum")
	ETeamSpeak_ACLType ACLType;

    static TeamSpeak_Manager* get_teampspeak_manager_ptr();

private:
	static TSharedPtr<TeamSpeak_Manager> ts_Manager;
	static UTeamSpeakFunctionLibrary* utsFL;
};

enum class ETeamSpeak_Error
{
    ERROR_ok = 0x0000,
    ERROR_undefined = 0x0001,
    ERROR_not_implemented = 0x0002,
    ERROR_ok_no_update = 0x0003,
    ERROR_dont_notify = 0x0004,
    ERROR_lib_time_limit_reached = 0x0005,

    //dunno
    ERROR_command_not_found = 0x0100,
    ERROR_unable_to_bind_network_port = 0x0101,
    ERROR_no_network_port_available = 0x0102,

    //client
    ERROR_client_invalid_id = 0x0200,
    ERROR_client_nickname_inuse = 0x0201,
    ERROR_client_protocol_limit_reached = 0x0203,
    ERROR_client_invalid_type = 0x0204,
    ERROR_client_already_subscribed = 0x0205,
    ERROR_client_not_logged_in = 0x0206,
    ERROR_client_could_not_validate_identity = 0x0207,
    ERROR_client_version_outdated = 0x020a,
    ERROR_client_is_flooding = 0x020c,
    ERROR_client_hacked = 0x020d,
    ERROR_client_cannot_verify_now = 0x020e,
    ERROR_client_login_not_permitted = 0x020f,
    ERROR_client_not_subscribed = 0x0210,

    //channel
    ERROR_channel_invalid_id = 0x0300,
    ERROR_channel_protocol_limit_reached = 0x0301,
    ERROR_channel_already_in = 0x0302,
    ERROR_channel_name_inuse = 0x0303,
    ERROR_channel_not_empty = 0x0304,
    ERROR_channel_can_not_delete_default = 0x0305,
    ERROR_channel_default_require_permanent = 0x0306,
    ERROR_channel_invalid_flags = 0x0307,
    ERROR_channel_parent_not_permanent = 0x0308,
    ERROR_channel_maxclients_reached = 0x0309,
    ERROR_channel_maxfamily_reached = 0x030a,
    ERROR_channel_invalid_order = 0x030b,
    ERROR_channel_no_filetransfer_supported = 0x030c,
    ERROR_channel_invalid_password = 0x030d,
    ERROR_channel_invalid_security_hash = 0x030f, //note 0x030e is defined in public_rare_errors,

                                                                     //server
    ERROR_server_invalid_id = 0x0400,
    ERROR_server_running = 0x0401,
    ERROR_server_is_shutting_down = 0x0402,
    ERROR_server_maxclients_reached = 0x0403,
    ERROR_server_invalid_password = 0x0404,
    ERROR_server_is_virtual = 0x0407,
    ERROR_server_is_not_running = 0x0409,
    ERROR_server_is_booting = 0x040a,
    ERROR_server_status_invalid = 0x040b,
    ERROR_server_version_outdated = 0x040d,
    ERROR_server_duplicate_running = 0x040e,

    //parameter
    ERROR_parameter_quote = 0x0600,
    ERROR_parameter_invalid_count = 0x0601,
    ERROR_parameter_invalid = 0x0602,
    ERROR_parameter_not_found = 0x0603,
    ERROR_parameter_convert = 0x0604,
    ERROR_parameter_invalid_size = 0x0605,
    ERROR_parameter_missing = 0x0606,
    ERROR_parameter_checksum = 0x0607,

    //unsorted, need further investigation
    ERROR_vs_critical = 0x0700,
    ERROR_connection_lost = 0x0701,
    ERROR_not_connected = 0x0702,
    ERROR_no_cached_connection_info = 0x0703,
    ERROR_currently_not_possible = 0x0704,
    ERROR_failed_connection_initialisation = 0x0705,
    ERROR_could_not_resolve_hostname = 0x0706,
    ERROR_invalid_server_connection_handler_id = 0x0707,
    ERROR_could_not_initialise_input_manager = 0x0708,
    ERROR_clientlibrary_not_initialised = 0x0709,
    ERROR_serverlibrary_not_initialised = 0x070a,
    ERROR_whisper_too_many_targets = 0x070b,
    ERROR_whisper_no_targets = 0x070c,

    //file transfer
    ERROR_file_invalid_name = 0x0800,
    ERROR_file_invalid_permissions = 0x0801,
    ERROR_file_already_exists = 0x0802,
    ERROR_file_not_found = 0x0803,
    ERROR_file_io_error = 0x0804,
    ERROR_file_invalid_transfer_id = 0x0805,
    ERROR_file_invalid_path = 0x0806,
    ERROR_file_no_files_available = 0x0807,
    ERROR_file_overwrite_excludes_resume = 0x0808,
    ERROR_file_invalid_size = 0x0809,
    ERROR_file_already_in_use = 0x080a,
    ERROR_file_could_not_open_connection = 0x080b,
    ERROR_file_no_space_left_on_device = 0x080c,
    ERROR_file_exceeds_file_system_maximum_size = 0x080d,
    ERROR_file_transfer_connection_timeout = 0x080e,
    ERROR_file_connection_lost = 0x080f,
    ERROR_file_exceeds_supplied_size = 0x0810,
    ERROR_file_transfer_complete = 0x0811,
    ERROR_file_transfer_canceled = 0x0812,
    ERROR_file_transfer_interrupted = 0x0813,
    ERROR_file_transfer_server_quota_exceeded = 0x0814,
    ERROR_file_transfer_client_quota_exceeded = 0x0815,
    ERROR_file_transfer_reset = 0x0816,
    ERROR_file_transfer_limit_reached = 0x0817,

    //sound
    ERROR_sound_preprocessor_disabled = 0x0900,
    ERROR_sound_internal_preprocessor = 0x0901,
    ERROR_sound_internal_encoder = 0x0902,
    ERROR_sound_internal_playback = 0x0903,
    ERROR_sound_no_capture_device_available = 0x0904,
    ERROR_sound_no_playback_device_available = 0x0905,
    ERROR_sound_could_not_open_capture_device = 0x0906,
    ERROR_sound_could_not_open_playback_device = 0x0907,
    ERROR_sound_handler_has_device = 0x0908,
    ERROR_sound_invalid_capture_device = 0x0909,
    ERROR_sound_invalid_playback_device = 0x090a,
    ERROR_sound_invalid_wave = 0x090b,
    ERROR_sound_unsupported_wave = 0x090c,
    ERROR_sound_open_wave = 0x090d,
    ERROR_sound_internal_capture = 0x090e,
    ERROR_sound_device_in_use = 0x090f,
    ERROR_sound_device_already_registerred = 0x0910,
    ERROR_sound_unknown_device = 0x0911,
    ERROR_sound_unsupported_frequency = 0x0912,
    ERROR_sound_invalid_channel_count = 0x0913,
    ERROR_sound_read_wave = 0x0914,
    ERROR_sound_need_more_data = 0x0915, //for internal purposes only
    ERROR_sound_device_busy = 0x0916, //for internal purposes only
    ERROR_sound_no_data = 0x0917,
    ERROR_sound_channel_mask_mismatch = 0x0918,


    //permissions
    ERROR_permissions_client_insufficient = 0x0a08,
    ERROR_permissions = 0x0a0c,

    //accounting
    ERROR_accounting_virtualserver_limit_reached = 0x0b00,
    ERROR_accounting_slot_limit_reached = 0x0b01,
    ERROR_accounting_license_file_not_found = 0x0b02,
    ERROR_accounting_license_date_not_ok = 0x0b03,
    ERROR_accounting_unable_to_connect_to_server = 0x0b04,
    ERROR_accounting_unknown_error = 0x0b05,
    ERROR_accounting_server_error = 0x0b06,
    ERROR_accounting_instance_limit_reached = 0x0b07,
    ERROR_accounting_instance_check_error = 0x0b08,
    ERROR_accounting_license_file_invalid = 0x0b09,
    ERROR_accounting_running_elsewhere = 0x0b0a,
    ERROR_accounting_instance_duplicated = 0x0b0b,
    ERROR_accounting_already_started = 0x0b0c,
    ERROR_accounting_not_started = 0x0b0d,
    ERROR_accounting_to_many_starts = 0x0b0e,

    //provisioning server
    ERROR_provisioning_invalid_password = 0x1100,
    ERROR_provisioning_invalid_request = 0x1101,
    ERROR_provisioning_no_slots_available = 0x1102,
    ERROR_provisioning_pool_missing = 0x1103,
    ERROR_provisioning_pool_unknown = 0x1104,
    ERROR_provisioning_unknown_ip_location = 0x1105,
    ERROR_provisioning_internal_tries_exceeded = 0x1106,
    ERROR_provisioning_too_many_slots_requested = 0x1107,
    ERROR_provisioning_too_many_reserved = 0x1108,
    ERROR_provisioning_could_not_connect = 0x1109,
    ERROR_provisioning_auth_server_not_connected = 0x1110,
    ERROR_provisioning_auth_data_too_large = 0x1111,
    ERROR_provisioning_already_initialized = 0x1112,
    ERROR_provisioning_not_initialized = 0x1113,
    ERROR_provisioning_connecting = 0x1114,
    ERROR_provisioning_already_connected = 0x1115,
    ERROR_provisioning_not_connected = 0x1116,
    ERROR_provisioning_io_error = 0x1117,
    ERROR_provisioning_invalid_timeout = 0x1118,
    ERROR_provisioning_ts3server_not_found = 0x1119,
    ERROR_provisioning_no_permission = 0x111A
};
