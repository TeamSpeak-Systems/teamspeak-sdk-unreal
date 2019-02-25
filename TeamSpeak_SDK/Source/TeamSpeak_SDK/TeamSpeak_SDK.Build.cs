using System.IO;
using UnrealBuildTool;

using System;

public class TeamSpeak_SDK : ModuleRules
{
	public TeamSpeak_SDK(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        string BaseDirectory = Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, "..", ".."));
        string SDKDirectory = Path.Combine(BaseDirectory, "ThirdParty");

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(SDKDirectory, "include")
				// ... add public include paths required here ...
			}
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                // Path.Combine(ModuleDirectory, "Private")
				// ... add other private include paths required here ...
			}
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Projects"
                // ... add other public dependencies that you statically link with here ...
            }
        );

		PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
                // ... add private dependencies that you statically link with here ...	
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

            }
        } else {
            PublicDefinitions.Add("WITH_TEAMSPEAK=0");
        }


        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }
	}
}
