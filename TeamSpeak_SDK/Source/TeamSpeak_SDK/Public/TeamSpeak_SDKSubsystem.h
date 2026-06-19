#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TeamSpeak_SDKSubsystem.generated.h"

UCLASS(meta = (DisplayName = "TeamSpeak SDK Subsystem"))
class TEAMSPEAK_SDK_API UTeamSpeak_SDKSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Outgoing VoiceClientComponent stashes its handler here on LevelTransition;
    // incoming component consumes it instead of opening a fresh connection.
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|CarryOver")
    void StashCarryOverHandler(int32 HandlerID);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|CarryOver")
    int32 ConsumeCarryOverHandler();

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|CarryOver")
    bool HasCarryOverHandler() const { return StashedHandlerID != 0; }

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Lifecycle")
    bool IsClientLibInitialized() const { return bClientLibInitialized; }

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Lifecycle")
    int32 GetLastInitError() const { return LastInitError; }

private:
    UPROPERTY(Transient)
    int32 StashedHandlerID = 0;

    bool bClientLibInitialized = false;
    int32 LastInitError = 0;
};
