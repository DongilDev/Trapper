// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TrapperProject : ModuleRules
{
	public TrapperProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "TrapperProject" });

        PublicDependencyModuleNames.AddRange(new string[]
       {
             "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
             "AIModule", "UMG", "Niagara", "NavigationSystem",
             "Sockets", "Networking", "AdvancedWidgets", "MovieScene",
             "LevelSequence", "AnimGraphRuntime", "RHI", "Slate", "MediaAssets",
             "Json","JsonUtilities", "AudioMixer"
       });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "TrapperProject/",
            "TrapperProject/Network/",
        });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
