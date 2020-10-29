// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#include "FiremeterBlueprintLibrary.h"

#include "Engine/World.h"
#include "Algo/Reverse.h"
#include "Toolkits/FConsoleCommandExecutor.h"

#include "Firemeter/Library/DebugUtility.h"

bool UFiremeterBlueprintLibrary::IsInEditor()
{
	return WITH_EDITOR;
}

bool UFiremeterBlueprintLibrary::IsDevelopmentBuild()
{
	// ReSharper disable once CppRedundantBooleanExpressionArgument
	return UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT;
}

bool UFiremeterBlueprintLibrary::IsReleaseBuild()
{
	// ReSharper disable once CppRedundantBooleanExpressionArgument
	return UE_BUILD_SHIPPING || UE_BUILD_TEST;
}

void UFiremeterBlueprintLibrary::ToggleUnitStats(bool bEnable)
{
#if WITH_EDITOR
	static bool bIsEnabled = false;
	static FConsoleCommandExecutor CommandExecutor; // We need to include UnrealEd as a module dependency (but only when targeting Editor)

	static auto CommandArray = TArray<TCHAR const*>{ {
		TEXT("stat FPS"),
		TEXT("stat Unit"),
		TEXT("stat UnitGraph")
	} };
	static auto ReverseCommandArray = [] (auto CommandArrayCopy) {
		Algo::Reverse(CommandArrayCopy);
		return CommandArrayCopy;
	}(CommandArray);

	static constexpr decltype(auto) StatCommandFormat = TEXT("stat %s");

	if (bIsEnabled != bEnable)
	{
		bIsEnabled = bEnable;
		if (bIsEnabled)
		{
			for (auto const StatCommand : CommandArray)
			{
				CommandExecutor.Exec(StatCommand);
			}
		}
		else
		{
			// There is some serious problem with this: ProfileGPU reports a second Scene draw after a call to this
			//CommandExecutor.Exec(TEXT("stat none"));
			
			for (auto const StatCommand : ReverseCommandArray) // We must execute these in reverse order
			{
				CommandExecutor.Exec(StatCommand);
			}
		}

		auto const & CustomStatGroups = FStatGroupRegistry::QueryStatGroups();

		for (auto const & StatGroup : CustomStatGroups)
		{
			CommandExecutor.Exec(*FString::Printf(StatCommandFormat, *StatGroup));
		}
	}
#else
	ensureMsgf(false, TEXT("This function should not be called on release builds!"));
#endif
}

void UFiremeterBlueprintLibrary::SetMaxFramerate(float NewFramerate)
{
#if WITH_EDITOR
	static FConsoleCommandExecutor CommandExecutor; // We need to include UnrealEd as a module dependency (but only when targeting Editor)

	static constexpr decltype(auto) MaxFPSCommandFormat = TEXT("t.MaxFPS %f");

	float const Framerate = FMath::Clamp(NewFramerate, 0.f, 300.f);
	
	CommandExecutor.Exec(*FString::Printf(MaxFPSCommandFormat, Framerate));
#else
	ensureMsgf(false, TEXT("This function should not be called on release builds!"));
#endif
}
