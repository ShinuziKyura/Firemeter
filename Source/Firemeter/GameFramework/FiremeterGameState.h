// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "FiremeterGameState.generated.h"

class ALifelineShip;

UCLASS()
class FIREMETER_API AFiremeterGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AFiremeterGameState(FObjectInitializer const & ObjectInitializer);
	
};
