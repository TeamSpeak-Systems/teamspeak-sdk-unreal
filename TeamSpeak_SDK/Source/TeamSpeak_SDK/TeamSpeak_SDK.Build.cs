using System.IO;
using UnrealBuildTool;

using System;

public class TeamSpeak_SDK : ModuleRules
{
    public TeamSpeak_SDK(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        string BaseDirectory = System.IO.Path.GetFullPath(
            System.IO.Path.Combine(ModuleDirectory, "..", "..")
            );

        string SDKDirectory = Path.Combine(BaseDirectory, "ThirdParty");

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(SDKDirectory, "include")
			}
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "AudioMixer",
                "SignalProcessing",
                "InputCore",
                "Projects"
            }
        );

        bool bHasTeamspeakSDK = System.IO.Directory.Exists(SDKDirectory);

        if (bHasTeamspeakSDK) {
            PublicDefinitions.Add("WITH_TEAMSPEAK=1");
            if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32) {
                // Add the import library
                string platform = Target.Platform == UnrealTargetPlatform.Win32 ? "win32" : "win64";
                string LibDirectory = Path.Combine(SDKDirectory, "lib", "windows", platform);

                PublicLibraryPaths.Add(LibDirectory);
                PublicAdditionalLibraries.Add("ts3client.lib");

                string SDKLibWindows = Path.Combine(SDKDirectory, "bin", "windows", platform, "ts3client.dll");

                string binariesDir = Path.Combine(BaseDirectory, "Binaries", Target.Platform.ToString());
                string filename = "ts3client.dll";

                if (!Directory.Exists(binariesDir))
                    Directory.CreateDirectory(binariesDir);

                string binPath = Path.Combine(binariesDir, filename);
                if (!File.Exists(Path.Combine(binariesDir, filename)))
                    File.Copy(SDKLibWindows, binPath, true);

                RuntimeDependencies.Add(binPath);
            } else if (Target.Platform == UnrealTargetPlatform.Linux) {
                string binariesDir = Path.Combine(BaseDirectory, "Binaries", Target.Platform.ToString());
                string filename = "libts3client.so";
                string binPath = Path.Combine(binariesDir, filename);
                string SDKLibLinux = Path.Combine(SDKDirectory, "bin", "linux", "amd64", filename);

                if (!Directory.Exists(binariesDir))
                    Directory.CreateDirectory(binariesDir);

                if (!File.Exists(Path.Combine(binPath)))
                    File.Copy(SDKLibLinux, binPath, true);

                RuntimeDependencies.Add(binPath);
                PublicAdditionalLibraries.Add(Path.Combine(SDKDirectory, "bin", "linux", "amd64", "libts3client.so"));
            }
        } else {
            PublicDefinitions.Add("WITH_TEAMSPEAK=0");
        }
	}
}
