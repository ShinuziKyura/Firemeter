// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#include "LifelineShip.h"

#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Firemeter/Components/LifelineGraphComponent.h"
#include "Firemeter/GameFramework/FiremeterGameState.h"
#include "Firemeter/Library/DebugUtility.h"
#include "Firemeter/Library/MathUtility.h"

DECLARE_LOG_CATEGORY_CLASS(LogLifelineShipPhysics, Log, All);

FM_DECLARE_STATS_GROUP(LifelineShipPhysics, TEXT("LifelineShip Physics"));
FM_DECLARE_CYCLE_STAT(LifelineShipPhysics, StatLinearMomentum, TEXT("Linear Momentum"));
FM_DECLARE_CYCLE_STAT(LifelineShipPhysics, StatAngularMomentum, TEXT("Angular Momentum"));

#define FM_SUPPRESS_LOG_FILEINFO

#define DEBUG_LIFELINESHIP_PHYSICS WITH_EDITOR && 1

ALifelineShip::ALifelineShip(FObjectInitializer const & ObjectInitializer)
	: Super{ ObjectInitializer }
	, LinearPower{ 50.f }
	, AngularPower{ 2.f }
	, AngularSway{ 1.f }
	, RollCorrectionFactor{ 4.f }
	, PitchRestrictionFactor{ 2.f }
	, LinearAxis{ 0.f }
	, AngularAxis{ 0.f }
	, bDisplayWorldAxis{ false }
	, bDisplayActorAxis{ false }
	, bDisplayRollCorrectionAxis{ false }
	, bDisplayPitchRestrictionAxis{ false }
	, bDisplayRawAngularAccelAxis{ false }
	, LifelineGraph{ ObjectInitializer.CreateDefaultSubobject<ULifelineGraphComponent>(this, TEXT("LifelineGraph")) }
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ALifelineShip::BeginPlay()
{
	Super::BeginPlay();

	auto const Primitive = Cast<UPrimitiveComponent>(RootComponent);
	if (!Primitive)
	{
		return;
	}

	PrimitiveComponent = Primitive;

	WeaponEnergyConsumption.Reserve(10);

#if 0
	SetActorTransform(Transform, false, nullptr, ETeleportType::ResetPhysics);
#endif
	SetActorTickEnabled(true);
}

void ALifelineShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hack to avoid access violations that may happen after too many live coding patches while in editor
	FM_LIVE_CODING_TICK_GUARD(
		[this] { return PrimitiveComponent.IsValid(); }, 
		[this] { PrimitiveComponent = Cast<UPrimitiveComponent>(RootComponent); }
	);

	// Calculate ship accel

	auto const Location = PrimitiveComponent->GetComponentLocation();
	auto const Orientation = PrimitiveComponent->GetComponentRotation();

	auto const LinearAccel = CalculateLinearAccel(Location, Orientation);
	auto const AngularAccel = CalculateAngularAccel(Location, Orientation);

	PrimitiveComponent->AddForce(LinearAccel, NAME_None, true);
	PrimitiveComponent->AddTorqueInRadians(AngularAccel, NAME_None,true);

	// Calculate ship energy

	CalculateWeaponEnergy(DeltaTime);
}

void ALifelineShip::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName(TEXT("LeftAxis_X")), this, &ALifelineShip::SetLeftAxisX);
	PlayerInputComponent->BindAxis(FName(TEXT("LeftAxis_Y")), this, &ALifelineShip::SetLeftAxisY);
	PlayerInputComponent->BindAxis(FName(TEXT("RightAxis_X")), this, &ALifelineShip::SetRightAxisX);
	PlayerInputComponent->BindAxis(FName(TEXT("RightAxis_Y")), this, &ALifelineShip::SetRightAxisY);
}

void ALifelineShip::AddWeaponEnergyConsumption(FWeaponEnergyConsumption const & InConsumption)
{
	WeaponEnergyConsumption.Add(InConsumption);
}

void ALifelineShip::SetLeftAxisX(float InLeftAxisX)
{
	LinearAxis.X = InLeftAxisX;
}

void ALifelineShip::SetLeftAxisY(float InLeftAxisY)
{
	LinearAxis.Y = InLeftAxisY;
}

void ALifelineShip::SetRightAxisX(float InRightAxisX)
{
	AngularAxis.X = InRightAxisX;
}

void ALifelineShip::SetRightAxisY(float InRightAxisY)
{
	AngularAxis.Y = InRightAxisY;
}

FVector ALifelineShip::CalculateLinearAccel(FVector const & InLocation, FRotator const & InOrientation) const
{
	SCOPE_CYCLE_COUNTER(StatLinearMomentum);

	FVector LinearAccel = FVector::ZeroVector;

	float const LinearCoefficient = LinearPower * 100.f;

	// Calculate roll correction sign

	float RollAbs = FMath::Abs(InOrientation.Roll * FiremeterMath::DegToRad);

	int32 const RollCorrectionSign = 2 * int32(RollAbs < FiremeterMath::HalfPi) - 1; // +1 when approaching 0, -1 when approaching pi

	// Calculate linear acceleration values

	float const LongitudinalAccel = LinearAxis.Y * LinearCoefficient;
	// We invert the direction of movement when we are flipped (when roll correction approaches pi),
	float const HorizontalAccel = LinearAxis.X * LinearCoefficient * RollCorrectionSign;
#if 0
	// We use the same value for both longitudinal and vertical movement,
	// as this seems more natural than controlling height using horizontal movements
	float const VerticalAccel = LinearAxis.Y * LinearCoefficient;
#else
	float const VerticalAccel = 0.f;
#endif

	// TODO query lifelines to constrain movement

//#if DEBUG_UNCONSTRAINED_LINEAR_MOVEMENT
	LinearAccel = FRotator{ 0.f, InOrientation.Yaw, 0.f }.RotateVector(FVector{ LongitudinalAccel, HorizontalAccel, VerticalAccel });
	//#endif

#if DEBUG_LIFELINESHIP_PHYSICS
	auto const World = GetWorld();

	if (bDisplayWorldAxis)
	{
		DrawDebugDirectionalArrow(World, InLocation, InLocation + FVector::ForwardVector * 100.f, 10.f, FColor::Red, false, -1.f, 192, 1.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + FVector::RightVector * 100.f, 10.f, FColor::Green, false, -1.f, 192, 1.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + FVector::UpVector * 100.f, 10.f, FColor::Blue, false, -1.f, 192, 1.f);
	}

	if (bDisplayActorAxis)
	{
		DrawDebugDirectionalArrow(World, InLocation, InLocation + InOrientation.RotateVector(FVector::ForwardVector * 100.f), 10.f, FColor::Red, false, -1.f, 192, 1.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + InOrientation.RotateVector(FVector::RightVector * 100.f), 10.f, FColor::Green, false, -1.f, 192, 1.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + InOrientation.RotateVector(FVector::UpVector * 100.f), 10.f, FColor::Blue, false, -1.f, 192, 1.f);
	}
#endif

	return LinearAccel;
}

FVector ALifelineShip::CalculateAngularAccel(FVector const & InLocation, FRotator const & InOrientation) const
{	
	SCOPE_CYCLE_COUNTER(StatAngularMomentum);

	FVector AngularAccel = FVector::ZeroVector;

	float const AngularCoefficient = AngularPower * FiremeterMath::Pi;

	// Calculate roll correction

	float const Roll = InOrientation.Roll * FiremeterMath::DegToRad;
	float const RollAbs = FMath::Abs(Roll);
	float const PiMinusRollAbs = FiremeterMath::Pi - RollAbs;

	int32 const RollSign = 2 * int32(Roll >= 0.f) - 1;
	int32 const ShouldCorrectRollNotPiMinusRoll = int32(RollAbs < PiMinusRollAbs);

	int32 const RollCorrectionSign = 2 * int32(RollAbs < FiremeterMath::HalfPi) - 1; // +1 when approaching 0, -1 when approaching pi

	// TODO add roll and yaw smoothing to roll correction when near tipping point

	float const RollCorrectionDelta = RollCorrectionSign * RollSign
		* (ShouldCorrectRollNotPiMinusRoll * RollAbs + (1 - ShouldCorrectRollNotPiMinusRoll) * PiMinusRollAbs)
		* RollCorrectionFactor * FiremeterMath::Pi;

	// Calculate pitch restriction

	float const Pitch = InOrientation.Pitch * FiremeterMath::DegToRad;
	float const PitchAbs = FMath::Abs(Pitch);

	int32 const ShouldRestrictPitch = int32(PitchAbs > FiremeterMath::OneThirdPi);

	float const PitchRestrictionDelta = Pitch * ShouldRestrictPitch
		* PitchRestrictionFactor * FiremeterMath::Pi;

	// Calculate yaw

	float const Yaw = InOrientation.Yaw * FiremeterMath::DegToRad;

	// Calculate angular acceleration values

	// We use the linear movement to introduce some sway (for aesthetic purposes),
	// roll correction stops this from growing indefinitely,
	// and we also reduce the effect for greater absolute pitch values
	float const RollAccel = -LinearAxis.X * AngularSway * FiremeterMath::Pi / (2.f * AngularSway * PitchAbs + 1.f);
	// Simple calculation, no adjustments needed
	float const PitchAccel = AngularAxis.Y * AngularCoefficient;
	// We invert the direction of movement when we are flipped (when roll correction approaches pi),
	// so the movement seems more natural
	float const YawAccel = AngularAxis.X * AngularCoefficient * RollCorrectionSign;

#if DEBUG_LIFELINESHIP_PHYSICS
	auto const World = GetWorld();

	auto const RollCorrection = InOrientation.RotateVector(FVector{ RollCorrectionDelta, 0.f, 0.f });
	auto const PitchRestriction = InOrientation.RotateVector(FVector{ 0.f, PitchRestrictionDelta, 0.f });

	auto const RawAngularAccel = InOrientation.RotateVector(FVector{ RollAccel, PitchAccel, YawAccel });

	AngularAccel = RollCorrection + PitchRestriction + RawAngularAccel;

	FM_ULOG(LogLifelineShipPhysics, TEXT("Roll: %f; Pitch: %f; Yaw: %f"),
		Roll, Pitch, Yaw);

	FM_ULOG(LogLifelineShipPhysics, TEXT("ShouldCorrectRollNotPiMinusRoll: %d; RollCorrectionDelta: %f"),
		ShouldCorrectRollNotPiMinusRoll, RollCorrectionDelta);

	FM_ULOG(LogLifelineShipPhysics, TEXT("ShouldRestrictPitch: %d; PitchRestrictionDelta: %f"),
		ShouldRestrictPitch, PitchRestrictionDelta);

	FM_ULOG(LogLifelineShipPhysics, TEXT("RollCorrection: %s; PitchRestriction: %s"),
		*RollCorrection.ToString(), *PitchRestriction.ToString());

	FM_ULOG(LogLifelineShipPhysics, TEXT("RawAngularAccel: %s; AngularAccel: %s"),
		*RawAngularAccel.ToString(), *AngularAccel.ToString());

	if (bDisplayRollCorrectionAxis)
	{
		auto const RollCorrectionQuat = FQuat{ RollCorrection.GetUnsafeNormal(), RollCorrection.Size() };

		DrawDebugDirectionalArrow(World, InLocation, InLocation + RollCorrectionQuat.RotateVector(FVector::ForwardVector * 50.f), 10.f, FColor::Magenta, false, -1.f, 193, 1.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + RollCorrectionQuat.RotateVector(FVector::RightVector * 50.f), 10.f, FColor::Magenta, false, -1.f, 193, 1.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + RollCorrectionQuat.RotateVector(FVector::UpVector * 50.f), 10.f, FColor::Magenta, false, -1.f, 193, 1.f);
	}

	if (bDisplayPitchRestrictionAxis)
	{
		DrawDebugDirectionalArrow(World, InLocation, InLocation + PitchRestriction * 10.f, 10.f, FColor::Yellow, false, -1.f, 193, 1.f);
	}

	if (bDisplayRawAngularAccelAxis)
	{
		auto const RawAngularAccelQuat = FQuat{ RawAngularAccel.GetUnsafeNormal(), RawAngularAccel.Size() };

		DrawDebugDirectionalArrow(World, InLocation, InLocation + RawAngularAccelQuat.RotateVector(FVector::ForwardVector * 50.f), 10.f, FColor::Cyan, false, -1.f, 194, 2.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + RawAngularAccelQuat.RotateVector(FVector::RightVector * 50.f), 10.f, FColor::Cyan, false, -1.f, 194, 2.f);
		DrawDebugDirectionalArrow(World, InLocation, InLocation + RawAngularAccelQuat.RotateVector(FVector::UpVector * 50.f), 10.f, FColor::Cyan, false, -1.f, 194, 2.f);
	}
#else
	AngularAccel =
		InOrientation.RotateVector(FVector{
			RollAccel + RollCorrectionDelta,
			PitchAccel + PitchRestrictionDelta,
			YawAccel
		});
#endif

	return AngularAccel;
}

void ALifelineShip::CalculateWeaponEnergy(float InDeltaTime)
{
	if (WeaponEnergyConsumption.Num())
	{
		for (int32 Index = 0; WeaponEnergyConsumption.IsValidIndex(Index); ++Index)
		{
			auto& Consumption = WeaponEnergyConsumption[Index];
			
			if ((Consumption.Duration -= InDeltaTime) > 0.f)
			{
				WeaponEnergy -= Consumption.Amount * InDeltaTime;
			}
			else
			{
				WeaponEnergy -= Consumption.Amount * (InDeltaTime + Consumption.Duration);
				WeaponEnergyConsumption.RemoveAt(Index--);
			}
		}
	}
	else
	{
		WeaponEnergy += WeaponEnergyRegeneration * InDeltaTime;
	}

	WeaponEnergy = FMath::Clamp(WeaponEnergy, 0.f, 1.f);
}
