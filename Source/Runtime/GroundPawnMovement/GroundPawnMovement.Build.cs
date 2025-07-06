// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GroundPawnMovement : ModuleRules
{
	public GroundPawnMovement(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		
		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"HelperFunctionLibrary", "NavigationSystem"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
