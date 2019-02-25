
using System.IO;
using UnrealBuildTool;

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
                string LibDirectory = Path.Combine(SDKDirectory, "lib");
                string platform = Target.Platform == UnrealTargetPlatform.Win32 ? "win32" : "win64";
                
                PublicLibraryPaths.Add(LibDirectory);
                PublicAdditionalLibraries.Add(Path.Combine("windows", platform, "ts3client.lib"));

                // Delay-load the DLL, so we can load it from the right place first
                PublicDelayLoadDLLs.Add("ts3client.dll");
                string SDKLibWindows = System.IO.Path.Combine(LibDirectory, "windows", platform,"ts3client.dll");
                RuntimeDependencies.Add(SDKLibWindows);
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
