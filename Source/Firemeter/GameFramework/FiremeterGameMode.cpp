// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#include "FiremeterGameMode.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

#include "Firemeter/GameFramework/FiremeterGameState.h"
#include "Firemeter/Actors/Pawns/PlayerShip.h"

AFiremeterGameMode::AFiremeterGameMode(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	GameStateClass = AFiremeterGameState::StaticClass();
	DefaultPawnClass = APlayerShip::StaticClass();
}

void AFiremeterGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AFiremeterGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
