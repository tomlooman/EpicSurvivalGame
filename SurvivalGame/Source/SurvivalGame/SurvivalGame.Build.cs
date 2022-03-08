// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SurvivalGame : ModuleRules
{
	public SurvivalGame(ReadOnlyTargetRules Target) : base(Target)
    {
        /* AIModule is not assigned by default, must be added when dealing with AI in your project */
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "UMG",
            /* Temporarily added GameplayTasks to workaround 4.12 compilation bug. */
            "GameplayTasks",
            "NavigationSystem",
            "OnlineSubsystem", // For Steam
            "PhysicsCore" // PhysicalMaterials
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "KantanChartsSlate", "KantanChartsDatasource", "KantanChartsUMG", "Slate", "SlateCore" });
	}
}
