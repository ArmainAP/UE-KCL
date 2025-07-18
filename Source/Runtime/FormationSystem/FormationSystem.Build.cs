// Copyright to Kat Code Labs, SRL. All Rights Reserved.

using UnrealBuildTool;

public class FormationSystem : ModuleRules
{
	public FormationSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AIModule", "HelperFunctionLibrary"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
