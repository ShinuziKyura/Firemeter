// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

#include "LifelineShip.generated.h"

USTRUCT(BlueprintType)
struct FWeaponEnergyConsumption
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Amount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Duration;
};

UCLASS(Blueprintable)
class FIREMETER_API ALifelineShip : public APawn
{
	GENERATED_BODY()
	
public:
	ALifelineShip(FObjectInitializer const & ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent * PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Ship")
	void AddWeaponEnergyConsumption(FWeaponEnergyConsumption const & InConsumption);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Linear", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	float LinearPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Angular", meta = (ClampMin = "1.0", ClampMax = "10.0"))
	float AngularPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Angular", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float AngularSway;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Angular", meta = (ClampMin = "0.0", ClampMax = "20.0", 
		ToolTip = "Roll correction attempts to keep the Roll at either 0 or 180 degrees, depending on which value requires the smallest delta to reach"))
	float RollCorrectionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Angular", meta = (ClampMin = "0.0", ClampMax = "10.0", 
		ToolTip = "Pitch restriction attempts to keep the Pitch from going over 60 or -60 degrees"))
	float PitchRestrictionFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics|Input")
	FVector2D LinearAxis;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics|Input")
	FVector2D AngularAxis;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ship")
	float ShipIntegrity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ship")
	float EngineEnergy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ship")
	float WeaponEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Ship")
	float WeaponEnergyRegeneration;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ship")
	TArray<FWeaponEnergyConsumption> WeaponEnergyConsumption;

private:
	UFUNCTION()
    void SetLeftAxisX(float InLeftAxisX);
	UFUNCTION()
    void SetLeftAxisY(float InLeftAxisY);
	UFUNCTION()
    void SetRightAxisX(float InRightAxisX);
	UFUNCTION()
    void SetRightAxisY(float InRightAxisY);

	FVector CalculateLinearAccel(FVector const & InLocation, FRotator const & InOrientation) const;
	FVector CalculateAngularAccel(FVector const & InLocation, FRotator const & InOrientation) const;

	void CalculateWeaponEnergy(float InDeltaTime);

	UPROPERTY(EditAnywhere, Category = "Physics|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDisplayWorldAxis;
	UPROPERTY(EditAnywhere, Category = "Physics|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDisplayActorAxis;
	
	UPROPERTY(EditAnywhere, Category = "Physics|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDisplayRollCorrectionAxis;
	UPROPERTY(EditAnywhere, Category = "Physics|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDisplayPitchRestrictionAxis;
	
	UPROPERTY(EditAnywhere, Category = "Physics|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDisplayRawAngularAccelAxis;

	UPROPERTY()
	class ULifelineGraphComponent* LifelineGraph;
	
	// Cached pointer to RootComponent cast to UPrimitiveComponent
	TWeakObjectPtr<class UPrimitiveComponent> PrimitiveComponent;
	
};

