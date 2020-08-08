// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class toadEditor : ModuleRules
{
	public toadEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UnrealED",
			"AnimGraph",
			"AnimGraphRuntime",
			"BlueprintGraph"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "toad" });

		PrivateDependencyModuleNames.AddRange(new string[] { });
		//Only if you created the Public and Private folder paths
		PublicIncludePaths.AddRange(
			new string[]
			{
				"toadEditor/Public"
			});

		PrivateIncludePaths.AddRange(
			new string[]
			{
				"toadEditor/Private"
			});
	}
}
