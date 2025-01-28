using UnrealBuildTool;

public class ProceduralAlgorithmsEditor : ModuleRules
{
	public ProceduralAlgorithmsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"AssetRegistry", 
			"ProceduralAlgorithms", 
			"Slate", 
			"SlateCore",
			"UnrealEd",        					// Required for editor utilities
    	"Blutility",       					// For UAssetActionUtility
    	"EditorScriptingUtilities" 	// For advanced editor scripting functions
		});	
	}
}