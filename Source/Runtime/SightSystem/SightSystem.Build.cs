// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SightSystem : ModuleRules
{
	public SightSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine", "AIModule"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
