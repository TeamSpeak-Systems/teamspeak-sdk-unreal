using System.IO;
using UnrealBuildTool;

using System;

public class TeamSpeak_SDK : ModuleRules
{
	public TeamSpeak_SDK(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public")
			}
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Projects",
                "AudioMixer",
                "SignalProcessing",
                "TeamSpeak_SDK_Core"
            }
        );
	}
}
