using UnrealBuildTool;

public class ProceduralAlgorithms : ModuleRules
{
	public ProceduralAlgorithms(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "AssetRegistry", "ProceduralMeshComponent"});	
	}
}