// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#include "LifelineGraphComponent.h"

#include "Firemeter/Actors/Pawns/LifelineShip.h"
#include "Firemeter/Actors/Lifeline.h"
#include "Firemeter/Library/DebugUtility.h"

ULifelineGraphComponent::ULifelineGraphComponent(FObjectInitializer const & ObjectInitializer)
	: Super{ ObjectInitializer }
{
	if (!IsTemplate())
	{
		RootNodes.Reserve(4);

		auto const Ship = Cast<ALifelineShip>(GetOuter());
		ensure(Ship);
		
		LifelineSpawnParams.Instigator = Ship;
		LifelineSpawnParams.Owner = Ship;
		LifelineSpawnParams.bDeferConstruction = true;
	}
}

void ULifelineGraphComponent::SpawnLifeline(FVector const & In_Origin, FVector const & In_Direction, float In_Length, float In_Duration)
{
	ensure(In_Direction.IsUnit());
	
	auto const Lifeline = GetWorld()->SpawnActor<ALifeline>(LifelineSpawnParams);
	auto const Transform = Lifeline->Init(In_Origin, In_Direction, In_Length, In_Duration);
	Lifeline->FinishSpawning(Transform);

	//CurrentNode->InsertLifelineNode(Lifeline);
}

void ULifelineGraphComponent::BeginPlay()
{
	Super::BeginPlay();

	auto const MainLifeline = GetWorld()->SpawnActor<ALifeline>(LifelineSpawnParams);
	auto const Transform = MainLifeline->Init();
	MainLifeline->FinishSpawning(Transform);

	CurrentNode = &RootNodes.Add_GetRef(FLifelineGraphNode{ MainLifeline });
}
