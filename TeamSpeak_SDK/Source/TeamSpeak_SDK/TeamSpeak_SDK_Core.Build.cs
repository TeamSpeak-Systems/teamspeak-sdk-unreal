using System.IO;
using System;

namespace UnrealBuildTool.Rules
{
    public class TeamSpeak_SDK_Core : ModuleRules
    {
        public TeamSpeak_SDK_Core(ReadOnlyTargetRules Target) : base(Target)
        {
            // Tell Unreal that this Module only imports Third-Party-Assets
            Type = ModuleType.External;

            string BaseDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
            string SDKDirectory = Path.Combine(BaseDirectory, "ThirdParty");

            string PlatformDir = GetPlatformDir(Target);
            if (string.IsNullOrEmpty(PlatformDir))
                throw new BuildException("TeamSpeak SDK: unsupported target platform " + Target.Platform);

            string PlatformRoot = Path.Combine(SDKDirectory, PlatformDir);
            if (!Directory.Exists(PlatformRoot))
                throw new BuildException(
                    "TeamSpeak SDK missing at '" + PlatformRoot +
                    "'. Populate ThirdParty/ from an SDK drop before building.");

            PublicIncludePaths.Add(Path.Combine(PlatformRoot, "include"));

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicAdditionalLibraries.Add(Path.Combine(PlatformRoot, "lib", "teamspeak_sdk_client.lib"));

                string Dll = Path.Combine(PlatformRoot, "bin", "teamspeak_sdk_client.dll");
                RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "teamspeak_sdk_client.dll"), Dll);
            }
            else if (Target.Platform == UnrealTargetPlatform.Linux
                     || Target.Platform == UnrealTargetPlatform.LinuxArm64)
            {
                string So = Path.Combine(PlatformRoot, "lib", "libteamspeak_sdk_client.so");
                PublicAdditionalLibraries.Add(So);
                RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "libteamspeak_sdk_client.so"), So);
            }
            else if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                string Dylib = Path.Combine(PlatformRoot, "lib", "libteamspeak_sdk_client.dylib");
                if (File.Exists(Dylib))
                {
                    PublicAdditionalLibraries.Add(Dylib);
                    RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "libteamspeak_sdk_client.dylib"), Dylib);
                }
            }
        }

        private static string GetPlatformDir(ReadOnlyTargetRules Target)
        {
            if (Target.Platform == UnrealTargetPlatform.Win64)
                return "windows-x86_64";
            if (Target.Platform == UnrealTargetPlatform.Linux)
                return "linux-x86_64";
            if (Target.Platform == UnrealTargetPlatform.LinuxArm64)
                return "linux-armv8";
            if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                return "macos-armv8";
            }
            if (Target.Platform == UnrealTargetPlatform.IOS)
                return "ios";
            if (Target.Platform == UnrealTargetPlatform.Android)
            {
                return "android-arm64-v8a";
            }
            return null;
        }
    }
}
