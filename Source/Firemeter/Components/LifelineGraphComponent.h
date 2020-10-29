// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "LifelineGraphComponent.generated.h"

USTRUCT(BlueprintType)
struct FIREMETER_API FLifelineGraphNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ALifeline* Lifeline;

	//TDoubleLinkedList<FLifelineGraphNode> LinkedNodes; // This should be ordered according to lifeline
	
};

UCLASS()
class FIREMETER_API ULifelineGraphComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	ULifelineGraphComponent(FObjectInitializer const & ObjectInitializer);

	void SpawnLifeline(FVector const & In_Origin, FVector const & In_Direction, float In_Length, float In_Duration);
	
protected:
	virtual void BeginPlay() override;
	
private:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FLifelineGraphNode* CurrentNode;

	TArray<FLifelineGraphNode> RootNodes; // This list always has at least a immutable head element, which is the main lifeline
	
	FActorSpawnParameters LifelineSpawnParams;
	
};
