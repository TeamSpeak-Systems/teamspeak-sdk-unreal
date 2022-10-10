using System.IO;
using System;

// #define TEAMSPEAK_COPY_DYNAMIC_SOUNDBACKENDS

namespace UnrealBuildTool.Rules
{
    public class TeamSpeak_SDK_Core : ModuleRules
    {
        private string BinariesPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../Binaries/")); }
        }

        public TeamSpeak_SDK_Core(ReadOnlyTargetRules Target) : base(Target)
        {
            // Tell Unreal that this Module only imports Third-Party-Assets
            Type = ModuleType.External;

            string BaseDirectory = Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, "..", ".."));
            string SDKDirectory = Path.Combine(BaseDirectory, "ThirdParty");

            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(SDKDirectory, "include")
                }
            );

            bool hasTeamspeakSDK = System.IO.Directory.Exists(SDKDirectory);
            PublicDefinitions.AddRange(
                new string[] {
                    string.Format("WITH_TEAMSPEAK={0}", hasTeamspeakSDK ? 1 : 0)
                }
            );
            if (hasTeamspeakSDK)
            {
#if UE_5_0_OR_LATER
                if (Target.Platform == UnrealTargetPlatform.Win64)
#else
                if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
#endif
                {
                    // Add the import library
                    string platform = Target.Platform == UnrealTargetPlatform.Win64 ? "win64" : "win32";
                    string LibDirectory = Path.Combine(SDKDirectory, "lib", "windows", platform);

                    // Explicitly name the used libraries
                    PublicAdditionalLibraries.Add(Path.Combine(LibDirectory, "ts3client.lib"));

                    string BinDir = Path.Combine(SDKDirectory, "bin", "windows", platform);
                    string ClientLibBin = "ts3client.dll";

                    /* "$(ModuleDir)" and the other string variables only work for RuntimeDependencies.Add,
                        so don't try using it for PublicIncludePaths.Add or any other .build.cs functions.
                        For those, the ModuleDirectory variable is all you have as far as I know. */
                    string Source = Path.Combine(BinDir, ClientLibBin);
                    string Destination = Path.Combine("$(BinaryOutputDir)", ClientLibBin);

                    RuntimeDependencies.Add(Destination, Source);
#if TEAMSPEAK_COPY_DYNAMIC_SOUNDBACKENDS
                    Copy_Soundbackends(BinDir);
#endif
                }
                else if (Target.Platform == UnrealTargetPlatform.Linux)
                {
                    string BinDir = Path.Combine(SDKDirectory, "bin", "linux", "amd64");
                    string ClientLibBin = "libts3client.so";

                    string Source = Path.Combine(BinDir, ClientLibBin);
                    string Destination = Path.Combine("$(BinaryOutputDir)", ClientLibBin);

                    RuntimeDependencies.Add(Destination, Source);
                    // PublicAdditionalLibraries.Add(Path.Combine(SDKDirectory, "bin", "linux", "amd64", "libts3client.so"));
#if TEAMSPEAK_COPY_DYNAMIC_SOUNDBACKENDS
                    Copy_Soundbackends(BinDir);
#endif
                }
            }
        }

        private void Copy_Soundbackends(string BinDir)
        {
            string soundbackends = Path.Combine(BinDir, "soundbackends");
            var files = Directory.GetFiles(soundbackends);
            foreach (string filePath in files)
            {
                string filename = Path.GetFileName(filePath);
                string Source = Path.Combine(soundbackends, filename);
                string Destination = Path.Combine("$(BinaryOutputDir)", "soundbackends", filename);
                RuntimeDependencies.Add(Destination, Source);
            }
        }
    }
}
