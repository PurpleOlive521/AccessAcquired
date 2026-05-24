// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

using UnrealBuildTool;

public class VerticalSliceEditor: ModuleRules
{
    public VerticalSliceEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {
                            "Core", 
                            "CoreUObject", 
                            "Engine", 
                            "GameplaySystem", 
                            "GameplaySystemEditor",
                            "Slate", 
                            "SlateCore",
                            "AssetTools",
                            "ClassViewer",
                            "GameplayTags",
                            "GameplayTagsEditor",
                            "InputCore",
                            "PropertyEditor",
                            "BlueprintGraph",
                            "Kismet",
                            "KismetCompiler",
                            "GraphEditor",
                            "LevelSequence",
                            "MainFrame",
                            "EditorFramework",
                            "UnrealEd",
                            "WorkspaceMenuStructure",
                            "ContentBrowser",
                            "EditorWidgets",
                            "SourceControl",
                            "ToolMenus"
        });
    }
}