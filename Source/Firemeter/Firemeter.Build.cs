// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Firemeter : ModuleRules
{
	public Firemeter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoPCHs;

        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine", "InputCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[]
        {

        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new []
            {
                "UnrealEd",
            });
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
