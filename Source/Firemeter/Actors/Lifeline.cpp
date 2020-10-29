// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#include "Lifeline.h"

#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

#include "Firemeter/Firemeter.h"
#include "Firemeter/Actors/Pawns/LifelineShip.h"
#include "Firemeter/Library/DebugUtility.h"

FM_DECLARE_STATS_GROUP(LifelinePhysics, TEXT("Lifeline Physics"));
FM_DECLARE_CYCLE_STAT(LifelinePhysics, StatLifelineTrace, TEXT("Lifeline Trace"));

namespace Lifeline_Internal
{
	TMap<FString, uint32> UniqueNameMap;
}

ALifeline::ALifeline(const FObjectInitializer & ObjectInitializer)
	: Super{ ObjectInitializer }
	, LifelineTraceParams{ TEXT(""), GET_STATID(StatLifelineTrace), false, this }
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CollisionCapsule"));
	CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_Lifeline);
	RootComponent = CollisionComponent.Get();
}

FTransform ALifeline::Init()
{
	Origin = FMath::VRand() * FMath::FRandRange(0.f, 10000.f);
	Direction = FMath::VRand();
	Length = -1.f;
	Duration = -1.f;

	return FTransform{ FQuat::FindBetweenNormals(FVector::ForwardVector, Direction), Origin };
}

FTransform ALifeline::Init(FVector const & In_Origin, FVector const & In_Direction, float In_Length, float In_Duration)
{
	Origin = In_Origin;
	Direction = In_Direction;
	Length = In_Length;
	Duration = In_Duration;

	return FTransform{ FQuat::FindBetweenNormals(FVector::ForwardVector, Direction), Origin };
}

void ALifeline::BeginPlay()
{
	Super::BeginPlay();

	auto const OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		OnOwnerShipDestroyed(nullptr);
		return;
	}

	OwnerActor->OnDestroyed.AddDynamic(this, &ALifeline::OnOwnerShipDestroyed);

	// TODO Extract to function
	auto const World = GetWorld();
	check(World);

	LifelineTraceDelegate.BindUObject(this, &ALifeline::HandleLifelineTrace);
	LifelineTraceParams.TraceTag = GenerateTraceTag();
	//FCollisionResponseParams LifelineTraceResponse{ ECR_Overlap };
	World->AsyncLineTraceByChannel(EAsyncTraceType::Multi, Origin, Origin + Direction * Length, ECollisionChannel::ECC_Projectile, LifelineTraceParams, FCollisionResponseParams::DefaultResponseParam, &LifelineTraceDelegate);

	// TODO Set aspect based on vars (maybe do this using particle effects)
	DrawDebugLine(GetWorld(), Origin - 100000.f * Direction, Origin + 100000.f * Direction, FColor::Purple, true, -1.f, 200, 10.f);

	if (Duration > 0.f)
	{
		//SetLifeSpan(Duration); // TODO Maybe we can use LifeSpan to control the lifetime of this lifeline
		SetActorTickEnabled(true);
	}
	
}

void ALifeline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALifeline::OnOwnerShipDestroyed(AActor * DestroyedActor)
{
	GetWorld()->DestroyActor(this);
}

void ALifeline::HandleLifelineTrace(const FTraceHandle& TraceHandle, FTraceDatum& TraceResults)
{
	for (auto const& HitResult : TraceResults.OutHits)
	{
		// TODO
		
		auto const Lifeline = Cast<ALifeline>(HitResult.Actor);
		if (Lifeline && Lifeline->GetOuter() == GetOuter())
		{
			
		}

		auto const Ship = Cast<ALifelineShip>(HitResult.Actor);
		if (Ship && Ship->GetOuter() != GetOuter())
		{

		}
	}
}

FName ALifeline::GenerateTraceTag() const
{
	auto const ObjectName = GetOwner()->GetName();
	auto const NameIndex = Lifeline_Internal::UniqueNameMap.FindOrAdd(ObjectName)++;
	
	return *FString::Printf(TEXT("%s_LifelineTrace_%d"), *ObjectName, NameIndex);
}

