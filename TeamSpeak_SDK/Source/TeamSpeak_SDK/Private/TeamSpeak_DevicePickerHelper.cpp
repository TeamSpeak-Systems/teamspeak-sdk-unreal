#include "TeamSpeak_DevicePickerHelper.h"

#include "TeamSpeak_VoiceClientComponent.h"
#include "TeamSpeakFunctionLibrary.h"


static TArray<FTeamSpeakDeviceInfo> EnumerateDevices(bool bCapture)
{
    TArray<FTeamSpeakDeviceInfo> Out;

    int32 ModeError = 0;
    const TArray<FString> Modes = bCapture
        ? UTeamSpeakFunctionLibrary::TeamSpeak_getCaptureModeList(ModeError)
        : UTeamSpeakFunctionLibrary::TeamSpeak_getPlaybackModeList(ModeError);

    if (ModeError != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
    {
        return Out;
    }

    for (const FString& Mode : Modes)
    {
        if (Mode.Equals(TEXT("custom"), ESearchCase::IgnoreCase))
        {
            continue;
        }

        int32 DeviceError = 0;
        const TArray<FTeamSpeak_2dArray> Devices = bCapture
            ? UTeamSpeakFunctionLibrary::TeamSpeak_getCaptureDeviceList(Mode, DeviceError)
            : UTeamSpeakFunctionLibrary::TeamSpeak_getPlaybackDeviceList(Mode, DeviceError);

        if (DeviceError != static_cast<int32>(ETeamSpeak_Error::ERROR_ok))
        {
            continue;
        }

        // SDK 2dArray entries are [DisplayName, DeviceID].
        for (const FTeamSpeak_2dArray& Entry : Devices)
        {
            if (Entry.value.Num() < 2)
            {
                continue;
            }
            FTeamSpeakDeviceInfo Info;
            Info.DisplayName = Entry.value[0];
            Info.DeviceID    = Entry.value[1];
            Info.BackendMode = Mode;
            Out.Add(MoveTemp(Info));
        }
    }

    return Out;
}

TArray<FTeamSpeakDeviceInfo> UTeamSpeak_DevicePickerHelper::GetCaptureDevices()
{
    return EnumerateDevices(/*bCapture=*/ true);
}

TArray<FTeamSpeakDeviceInfo> UTeamSpeak_DevicePickerHelper::GetPlaybackDevices()
{
    return EnumerateDevices(/*bCapture=*/ false);
}

void UTeamSpeak_DevicePickerHelper::ApplyCaptureDevice(UTeamSpeak_VoiceClientComponent* VoiceClient,
                                                      const FString& BackendMode,
                                                      const FString& DeviceID)
{
    if (!VoiceClient)
    {
        return;
    }
    VoiceClient->CaptureDeviceMode = BackendMode;
    VoiceClient->CaptureDeviceName = DeviceID;
    VoiceClient->ReopenCaptureDevice();
}

void UTeamSpeak_DevicePickerHelper::ApplyAECReferenceDevice(UTeamSpeak_VoiceClientComponent* VoiceClient,
                                                           const FString& BackendMode,
                                                           const FString& DeviceID)
{
    if (!VoiceClient)
    {
        return;
    }
    VoiceClient->AECReferenceDeviceMode = BackendMode;
    VoiceClient->AECReferenceDeviceID   = DeviceID;
    // AEC re-binding happens inside OpenCaptureDevice, so reopen.
    VoiceClient->ReopenCaptureDevice();
}
