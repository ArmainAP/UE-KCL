// Copyright to Kat Code Labs, SRL. All Rights Reserved.

using UnrealBuildTool;

public class SpawnHelpers : ModuleRules
{
	public SpawnHelpers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"FormationSystem"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
