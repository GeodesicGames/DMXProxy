// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DMXProxyRuntime : ModuleRules
{
	public DMXProxyRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Networking",
				"Sockets",
			}
		);

		if (Target.Type == TargetType.Editor)
		{
			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
					"Settings",
				});

			PrivateIncludePathModuleNames.AddRange(
				new string[] {
					"Settings",
				});
		}

	}
}
