// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProceduralDemoEditor : ModuleRules
{
	public ProceduralDemoEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
        PublicIncludePaths.Add("ProceduralDemoEditor/Public");
        PrivateIncludePaths.Add("ProceduralDemoEditor/Private");

		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore",
            "UnrealEd",
            "DetailCustomizations",
            "PropertyEditor",
            "EditorStyle",
            "ProceduralDemo"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { 
            "Slate", 
            "SlateCore" 
        });
	}
}
