
using System.IO;
using UnrealBuildTool;

public class TeamSpeak_SDK : ModuleRules
{
	public TeamSpeak_SDK(TargetInfo Target)
	{		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

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
        LoadTeamSpeakLib(Target);
	}

    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }    
 
    public bool LoadTeamSpeakLib(TargetInfo Target) {
        bool isLibrarySupported = false;

		const string ts3clientlib = "ts3client.lib";
		const string ts3clientdll = "ts3client.dll";

		string arch = null;

		if (Target.Platform == UnrealTargetPlatform.Win64) {
			isLibrarySupported = true;

			arch = "win64";

		} else if (Target.Platform == UnrealTargetPlatform.Win32) {
			isLibrarySupported = true;

			arch = "win32";

		}
        
        if(isLibrarySupported) {
			var LibrariesPath = Path.Combine(ThirdPartyPath, "lib", "windows", arch);
			var path = CopyToBinaries(Path.Combine(ThirdPartyPath, "bin", "windows", arch, ts3clientdll), Target);
			var dllPath = new RuntimeDependency(path);

			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, ts3clientlib));
			RuntimeDependencies.Add(dllPath);
			PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));
			
			Definitions.Add(string.Format( "WITH_BOBS_MAGIC_BINDING={0}", isLibrarySupported ? 1 : 0 ) );
 
			Definitions.Add("TS_MODULE_INTERNAL");
 
			PublicIncludePaths.Add(Path.Combine(GetUProjectPath(), "Plugins"));
			PublicIncludePaths.Add(Path.Combine(GetUProjectPath(), "Plugins/TeamSpeak_SDK/Source/TeamSpeak_SDK/Public"));
        }
		
        return isLibrarySupported;
    }

    public string GetUProjectPath()
    {
        return Directory.GetParent(ModulePath).Parent.FullName;
    }
    public string GetGameDir()
    {
        return Directory.GetParent(ModulePath).Parent.Parent.Parent.FullName;
    }

    private string CopyToBinaries(string Filepath, TargetInfo Target)
    {
        string binariesDir = Path.Combine(GetGameDir(), "Binaries", Target.Platform.ToString());
        string filename = Path.GetFileName(Filepath);

        if (!Directory.Exists(binariesDir))
            Directory.CreateDirectory(binariesDir);

        string binPath = Path.Combine(binariesDir, filename);
        if (!File.Exists(Path.Combine(binariesDir, filename)))
            File.Copy(Filepath, binPath, true);

        return binPath;
    }
}
