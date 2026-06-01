#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamSpeakFunctionLibrary.h"
#include "TeamSpeak_VoiceSpatialComponent.generated.h"

class APawn;
class UTeamSpeak_VoiceClientComponent;
class UTeamSpeak_SynthComponent_Client;

/**
 * Per-peer spatial voice glue. Attach to any Pawn that represents a player.
 *
 * On the locally-controlled instance, queries the local TeamSpeak client ID after
 * STATUS_CONNECTION_ESTABLISHED and pushes it via ServerRPC. On non-locally-controlled
 * instances (remote peers as seen by other clients), spawns a UTeamSpeak_SynthComponent_Client,
 * attaches it to the owner pawn's root, and assigns it to the replicated client ID so
 * per-peer audio is spatialized at the pawn's location.
 *
 * Requires the viewing client's PlayerController to host a UTeamSpeak_VoiceClientComponent
 * (the connection handler). The spatial component pulls the SDK handler from there.
 */
UCLASS(ClassGroup = (TeamSpeak), meta = (BlueprintSpawnableComponent))
class TEAMSPEAK_SDK_API UTeamSpeak_VoiceSpatialComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTeamSpeak_VoiceSpatialComponent();
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TeamSpeakClientId, Category = "TeamSpeak|Spatial")
    int32 TeamSpeakClientId = 0;

    UFUNCTION(BlueprintPure, Category = "TeamSpeak|Spatial")
    int32 GetTeamSpeakClientId() const { return TeamSpeakClientId; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION()
    void OnRep_TeamSpeakClientId();

    UFUNCTION(Server, Reliable)
    void Server_SetTeamSpeakClientId(int32 InClientId);

    UFUNCTION()
    void HandleLocalVoiceConnectStatusChanged(EConnectStatus NewStatus, int32 ErrorNumber);

private:
    APawn* GetOwnerPawn() const;
    bool IsOwnerLocallyControlled() const;
    UTeamSpeak_VoiceClientComponent* GetLocalVoiceClient() const;
    void BindToLocalVoiceClient();
    void UnbindFromLocalVoiceClient();
    void TryPushLocalClientId();
    void EnsureRemoteSynth();
    void DetachRemoteSynth();

    UPROPERTY(Transient)
    TObjectPtr<UTeamSpeak_VoiceClientComponent> BoundVoiceClient = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UTeamSpeak_SynthComponent_Client> RemoteSynth = nullptr;
};
