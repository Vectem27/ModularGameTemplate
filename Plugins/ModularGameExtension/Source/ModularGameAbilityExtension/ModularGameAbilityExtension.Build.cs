// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

using UnrealBuildTool;

public class ModularGameAbilityExtension : ModuleRules
{
	public ModularGameAbilityExtension (ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputCore", 
				"EnhancedInput",
				"GameFeatures",
				"ModularGameplay",

				"ModularGameExtension",
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",

                "GameplayAbilities", 
				"GameplayTags", 
				"GameplayTasks",
            }
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
