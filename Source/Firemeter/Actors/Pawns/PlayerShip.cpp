// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#include "PlayerShip.h"

APlayerShip::APlayerShip(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
