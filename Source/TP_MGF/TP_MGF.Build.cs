// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TP_MGF : ModuleRules
{
	public TP_MGF(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] 
			{
                "TP_MGF"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] 
			{

            }
        );

        PublicDependencyModuleNames.AddRange(
			new string[] 
			{ 
                ////////////////////
				/* Engine Modules */
                ////////////////////

                // Core
				"Core", 
				"CoreUObject", 
				"Engine", 

                // Input
				"InputCore", 
				"EnhancedInput",

                // Gameplay
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks",
                "GameFeatures",
                "ModularGameplay",

                // Systems
                "DeveloperSettings",
                "SignificanceManager",
                
                /////////////////////
				/* Project Modules */
                /////////////////////

                // Gameplay
				"ModularGameplayActors",
                "CommonGame",
                "CommonLoadingScreen",

            }
        );

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{ 
				////////////////////
				/* Engine Modules */
                ////////////////////
                
                // UI
                "UMG",
                "Slate",
                "SlateCore",
                "GameSubtitles",
                "CommonUI",

                // Input
                "CommonInput",

                // Audio
                "AudioMixer",
                "AudioModulation",
            }
        );

        // Generate compile errors if using DrawDebug functions in test/shipping builds.
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");

        SetupGameplayDebuggerSupport(Target);

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
