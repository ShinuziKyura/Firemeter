// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CollisionQueryParams.h"
#include "WorldCollision.h"
#include "GameFramework/Actor.h"

#include "Lifeline.generated.h"

UCLASS()
class FIREMETER_API ALifeline : public AActor
{
	GENERATED_BODY()
	
public:	
	ALifeline(const FObjectInitializer& ObjectInitializer);
	
	FTransform Init();
	FTransform Init(FVector const & In_Origin, FVector const & In_Direction, float In_Length, float In_Duration);
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnOwnerShipDestroyed(AActor* DestroyedActor);
	
//	UFUNCTION()
	void HandleLifelineTrace(const FTraceHandle & TraceHandle, FTraceDatum & TraceResults);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	FVector	Origin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	FVector	Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float Length;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float Duration;

	FName GenerateTraceTag() const;

	FTraceDelegate LifelineTraceDelegate;
	FCollisionQueryParams LifelineTraceParams;
	
	TWeakObjectPtr<class UCapsuleComponent> CollisionComponent;
	
};
