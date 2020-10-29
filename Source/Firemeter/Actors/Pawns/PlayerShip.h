// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "Firemeter/Firemeter.h"

#include "Firemeter/Actors/Pawns/LifelineShip.h"

#include "PlayerShip.generated.h"

UCLASS()
class FIREMETER_API APlayerShip : public ALifelineShip
{
	GENERATED_BODY()

public:
	APlayerShip(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


private:
	

};
