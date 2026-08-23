// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

using System.IO;
using UnrealBuildTool;

public class VerticalSlice : ModuleRules
{
	public VerticalSlice(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",

			// Custom ones for the project:
			"UMG",
			"Slate",
			"SlateCore",
			"CommonInput",
			"CommonUI",
			"GameplayTags",
			"MotionWarping",
			"DeveloperSettings",
			"EnhancedInput",
			"GameplaySystem",
			"GameplayTasks",
            "NavigationSystem"

            });

		PrivateDependencyModuleNames.AddRange(new string[] { "FMODStudio", "Slate", "SlateCore", "GameplayMessageRuntime" });

		IncludeAllSubDirectories();
	}

    // Adds all subdirectories under Source/ProjectName
    private void IncludeAllSubDirectories()
    {
        AddDirectoriesRecursive(ModuleDirectory);
    }

    // Recursively finds and adds all subdirectories, starting from Source/ProjectName
    private void AddDirectoriesRecursive(string DirectoryPathToSearch)
    {
        foreach (string DirectoryPath in Directory.GetDirectories(DirectoryPathToSearch))
        {
            PublicIncludePaths.Add(DirectoryPath);
            AddDirectoriesRecursive(DirectoryPath);
        }
    }
}
