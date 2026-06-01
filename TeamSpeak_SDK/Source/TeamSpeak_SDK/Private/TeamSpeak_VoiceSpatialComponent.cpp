#include "TeamSpeak_VoiceSpatialComponent.h"

#include "TeamSpeak_VoiceClientComponent.h"
#include "TeamSpeakFunctionLibrary.h"
#include "audiobridge/TeamSpeak_SynthComponent_Client.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UTeamSpeak_VoiceSpatialComponent::UTeamSpeak_VoiceSpatialComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UTeamSpeak_VoiceSpatialComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UTeamSpeak_VoiceSpatialComponent, TeamSpeakClientId);
}

void UTeamSpeak_VoiceSpatialComponent::BeginPlay()
{
    Super::BeginPlay();

    BindToLocalVoiceClient();

    if (IsOwnerLocallyControlled())
    {
        // Catch the case where the connection was already established before the pawn spawned.
        if (UTeamSpeak_VoiceClientComponent* VC = GetLocalVoiceClient(); VC && VC->IsConnected())
        {
            TryPushLocalClientId();
        }
    }
    else if (TeamSpeakClientId != 0)
    {
        // Late join: replicated value arrived before BeginPlay; OnRep won't fire again.
        EnsureRemoteSynth();
    }
}

void UTeamSpeak_VoiceSpatialComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindFromLocalVoiceClient();
    DetachRemoteSynth();
    Super::EndPlay(EndPlayReason);
}

APawn* UTeamSpeak_VoiceSpatialComponent::GetOwnerPawn() const
{
    return Cast<APawn>(GetOwner());
}

bool UTeamSpeak_VoiceSpatialComponent::IsOwnerLocallyControlled() const
{
    APawn* P = GetOwnerPawn();
    return P && P->IsLocallyControlled();
}

UTeamSpeak_VoiceClientComponent* UTeamSpeak_VoiceSpatialComponent::GetLocalVoiceClient() const
{
    UWorld* W = GetWorld();
    if (!W) return nullptr;
    APlayerController* PC = UGameplayStatics::GetPlayerController(W, 0);
    if (!PC) return nullptr;
    return PC->FindComponentByClass<UTeamSpeak_VoiceClientComponent>();
}

void UTeamSpeak_VoiceSpatialComponent::BindToLocalVoiceClient()
{
    if (BoundVoiceClient) return;
    UTeamSpeak_VoiceClientComponent* VC = GetLocalVoiceClient();
    if (!VC) return;
    VC->OnConnectStatusChanged.AddDynamic(this, &UTeamSpeak_VoiceSpatialComponent::HandleLocalVoiceConnectStatusChanged);
    BoundVoiceClient = VC;
}

void UTeamSpeak_VoiceSpatialComponent::UnbindFromLocalVoiceClient()
{
    if (!BoundVoiceClient) return;
    BoundVoiceClient->OnConnectStatusChanged.RemoveDynamic(this, &UTeamSpeak_VoiceSpatialComponent::HandleLocalVoiceConnectStatusChanged);
    BoundVoiceClient = nullptr;
}

void UTeamSpeak_VoiceSpatialComponent::HandleLocalVoiceConnectStatusChanged(EConnectStatus NewStatus, int32 ErrorNumber)
{
    if (NewStatus != EConnectStatus::STATUS_CONNECTION_ESTABLISHED) return;

    if (IsOwnerLocallyControlled())
    {
        TryPushLocalClientId();
    }
    else
    {
        // Remote pawn was waiting for our local handler to come online.
        EnsureRemoteSynth();
    }
}

void UTeamSpeak_VoiceSpatialComponent::OnRep_TeamSpeakClientId()
{
    if (!IsOwnerLocallyControlled())
    {
        EnsureRemoteSynth();
    }
}

void UTeamSpeak_VoiceSpatialComponent::Server_SetTeamSpeakClientId_Implementation(int32 InClientId)
{
    if (TeamSpeakClientId == InClientId) return;
    TeamSpeakClientId = InClientId;
    // Replication delivers OnRep on remotes; fire manually for listen-server spatialization.
    OnRep_TeamSpeakClientId();
}

void UTeamSpeak_VoiceSpatialComponent::TryPushLocalClientId()
{
    UTeamSpeak_VoiceClientComponent* VC = GetLocalVoiceClient();
    if (!VC) return;
    const int32 Handler = VC->GetHandlerID();
    if (Handler == 0) return;
    int32 Error = 0;
    const int32 Id = UTeamSpeakFunctionLibrary::TeamSpeak_getClientID(Handler, Error);
    if (Error != static_cast<int32>(ETeamSpeak_Error::ERROR_ok) || Id == 0) return;
    Server_SetTeamSpeakClientId(Id);
}

void UTeamSpeak_VoiceSpatialComponent::EnsureRemoteSynth()
{
    if (RemoteSynth || TeamSpeakClientId == 0) return;

    UTeamSpeak_VoiceClientComponent* VC = GetLocalVoiceClient();
    if (!VC) return;
    const int32 Handler = VC->GetHandlerID();
    if (Handler == 0) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;
    USceneComponent* Root = Owner->GetRootComponent();
    if (!Root) return;

    UTeamSpeak_SynthComponent_Client* Synth = NewObject<UTeamSpeak_SynthComponent_Client>(Owner);
    if (!Synth) return;
    Synth->RegisterComponent();
    Synth->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    Synth->TeamSpeak_AssignSynthToClient(Handler, TeamSpeakClientId);
    Synth->Start();
    RemoteSynth = Synth;
}

void UTeamSpeak_VoiceSpatialComponent::DetachRemoteSynth()
{
    if (!RemoteSynth) return;
    RemoteSynth->TeamSpeak_DeassignSynthFromClient();
    RemoteSynth->DestroyComponent();
    RemoteSynth = nullptr;
}
