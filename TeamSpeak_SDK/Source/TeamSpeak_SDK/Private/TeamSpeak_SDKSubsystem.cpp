#include "TeamSpeak_SDKSubsystem.h"
#include "audiobridge/TeamSpeak_AudioBridge.h"

void UTeamSpeak_SDKSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UTeamSpeak_AudioBridge::TeamSpeak_AudioBridge_initClientLib(LastInitError);
    bClientLibInitialized = (LastInitError == 0);
}

void UTeamSpeak_SDKSubsystem::Deinitialize()
{
    if (bClientLibInitialized)
    {
        UTeamSpeak_AudioBridge::TeamSpeak_AudioBridge_shutdown();
        bClientLibInitialized = false;
    }
    StashedHandlerID = 0;
    Super::Deinitialize();
}

void UTeamSpeak_SDKSubsystem::StashCarryOverHandler(int32 HandlerID)
{
    StashedHandlerID = HandlerID;
}

int32 UTeamSpeak_SDKSubsystem::ConsumeCarryOverHandler()
{
    const int32 Out = StashedHandlerID;
    StashedHandlerID = 0;
    return Out;
}
