// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlaystageDemo4 : ModuleRules
{
	public PlaystageDemo4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Json" });
	}
}
