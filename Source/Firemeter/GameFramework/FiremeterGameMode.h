// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "FiremeterGameMode.generated.h"

UCLASS()
class FIREMETER_API AFiremeterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFiremeterGameMode(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
