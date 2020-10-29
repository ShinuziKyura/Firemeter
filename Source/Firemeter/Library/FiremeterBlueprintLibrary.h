// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FiremeterBlueprintLibrary.generated.h"

UCLASS()
class FIREMETER_API UFiremeterBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// meta = (WorldContext = "WorldContextObject")
	// UObject* WorldContextObject
	
public:
	UFUNCTION(BlueprintPure)
	static bool IsInEditor();

	UFUNCTION(BlueprintPure)
	static bool IsDevelopmentBuild();
	
	UFUNCTION(BlueprintPure)
	static bool IsReleaseBuild();

	UFUNCTION(BlueprintCallable, Category = "Development")
	static void ToggleUnitStats(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Development")
	static void SetMaxFramerate(float NewFramerate);
	
};
