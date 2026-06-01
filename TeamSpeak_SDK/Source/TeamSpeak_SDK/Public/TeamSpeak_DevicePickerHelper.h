#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TeamSpeak_DevicePickerHelper.generated.h"

class UTeamSpeak_VoiceClientComponent;

USTRUCT(BlueprintType)
struct FTeamSpeakDeviceInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "TeamSpeak")
    FString DisplayName;

    // Opaque backend device ID. Pass this to ApplyCaptureDevice / ApplyAECReferenceDevice or
    // write into the corresponding UPROPERTY on the VoiceClientComponent. Not user-friendly.
    UPROPERTY(BlueprintReadOnly, Category = "TeamSpeak")
    FString DeviceID;

    UPROPERTY(BlueprintReadOnly, Category = "TeamSpeak")
    FString BackendMode;
};

/**
 * Settings-menu helpers for picking the TeamSpeak capture / AEC-reference devices.
 *
 * Enumeration covers every native backend the SDK reports (skipping "custom"). Apply functions
 * write the chosen mode+ID into the matching UPROPERTYs on UTeamSpeak_VoiceClientComponent and
 * reopen the capture device so the change takes effect immediately.
 *
 * Playback is not pickable: the plugin renders SDK voice through UE's audio mixer via the
 * custom playback device "TeamSpeak_UE_Audio". UE's own output device choice is what the user
 * actually hears. GetPlaybackDevices is provided for AEC-reference enumeration only.
 */
UCLASS()
class TEAMSPEAK_SDK_API UTeamSpeak_DevicePickerHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Devices")
    static TArray<FTeamSpeakDeviceInfo> GetCaptureDevices();

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Devices")
    static TArray<FTeamSpeakDeviceInfo> GetPlaybackDevices();

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Devices")
    static void ApplyCaptureDevice(UTeamSpeak_VoiceClientComponent* VoiceClient,
                                   const FString& BackendMode,
                                   const FString& DeviceID);

    UFUNCTION(BlueprintCallable, Category = "TeamSpeak|Devices")
    static void ApplyAECReferenceDevice(UTeamSpeak_VoiceClientComponent* VoiceClient,
                                        const FString& BackendMode,
                                        const FString& DeviceID);
};
