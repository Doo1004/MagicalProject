// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MagicalProject : ModuleRules
{
	public MagicalProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "UMG", "Slate", "SlateCore" , "AIModule", "GameplayTasks" , "NavigationSystem" });
	}
}
