// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_PawnMovementComponent.h"
#include "Engine.h"
//#include "TD_Logging.h"


const float UTD_PawnMovementComponent::BRAKE_TO_STOP_VELOCITY = 10.f;

UTD_PawnMovementComponent::UTD_PawnMovementComponent(const FObjectInitializer& ObjectInitializer) : UPawnMovementComponent(ObjectInitializer)
{
	bEnablePhysicsInteraction = true;
	StandingDownwardForceScale = 1.0f;
	InitialPushForceFactor = 500.0f;
	PushForceFactor = 750000.0f;
	PushForcePointZOffsetFactor = -0.75f;
	bPushForceUsingZOffset = false;
	bPushForceScaledToMass = false;
	bScalePushForceToVelocity = true;

	TouchForceFactor = 1.0f;
	bTouchForceScaledToMass = true;
	MinTouchForce = -1.0f;
	MaxTouchForce = 250.0f;
	RepulsionForce = 2.5f;
	GroundFriction = 8.0f;
	MaxAcceleration = 2048.0f;
	MaxDeceleration = 2048.0f;
	MaxSpeed = 300.0f;
	CanMove = true;
	bConstrainToPlane = true;
	PlaneConstraintNormal = FVector(0,0,1);
}

void UTD_PawnMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UTD_PawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Make sure that everything is still valid, and that we are allowed to move.
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime) || !CanMove)
    {
        return;
    }

	// TODO: this is 0.5 because i have the hands and red cube hooked up at once
	const FVector InputVector = ConsumeInputVector() * 0.5f;
	FVector ScaledInputVector = InputVector.GetClampedToMaxSize(1.0f);

	TargetVelocity = ComputeTargetVelocity(ScaledInputVector);
	Acceleration = ComputeAcceleration(ScaledInputVector);
	CalcVelocity(DeltaTime, GroundFriction, ScaledInputVector);

	PerformMovement(DeltaTime);
}

FVector UTD_PawnMovementComponent::ComputeTargetVelocity(const FVector& InputVector) const
{
	return MaxSpeed * InputVector;
}

FVector UTD_PawnMovementComponent::ComputeAcceleration(const FVector& InputVector) const
{
	// Apply braking or deceleration
	const bool bZeroInput = InputVector.IsZero();

	if (bZeroInput)
	{
		return -MaxDeceleration * Velocity.GetSafeNormal();
	}

	return MaxAcceleration * InputVector;
}

void UTD_PawnMovementComponent::CalcVelocity(float DeltaTime, float Friction, FVector InputVector)
{
	const bool bZeroInput = InputVector.IsZero();
	if (bZeroInput) {
		Velocity -= Velocity.GetSafeNormal() * MaxDeceleration * DeltaTime;
	}
	else {
		Velocity = TargetVelocity;
	}
	/*Velocity += Acceleration * DeltaTime;

	const bool bVelocityOverMax = IsExceedingMaxSpeed(MaxSpeed);
	if (bVelocityOverMax)
	{
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
	}*/


	// Clamp to zero if nearly zero, or if below min threshold and braking.
	const float VSizeSq = Velocity.SizeSquared();
	if (VSizeSq <= KINDA_SMALL_NUMBER || (VSizeSq <= FMath::Square(BRAKE_TO_STOP_VELOCITY)))
	{
		Velocity = FVector::ZeroVector;
	}
}

void UTD_PawnMovementComponent::HandleImpact(const FHitResult& Impact, float TimeSlice, const FVector& MoveDelta)
{
	APawn* OtherPawn = Cast<APawn>(Impact.GetActor());
	if (OtherPawn)
	{
		NotifyBumpedPawn(OtherPawn);
	}

	if (bEnablePhysicsInteraction)
	{
		const FVector ForceAccel = Acceleration + (IsFalling() ? FVector(0.f, 0.f, GetGravityZ()) : FVector::ZeroVector);
		ApplyImpactPhysicsForces(Impact, ForceAccel, Velocity);
	}
}

void UTD_PawnMovementComponent::ApplyImpactPhysicsForces(const FHitResult& Impact, const FVector& ImpactAcceleration, const FVector& ImpactVelocity)
{
	if (bEnablePhysicsInteraction && Impact.bBlockingHit)
	{
		if (UPrimitiveComponent* ImpactComponent = Impact.GetComponent())
		{
			FBodyInstance* BI = ImpactComponent->GetBodyInstance(Impact.BoneName);
			if (BI != nullptr && BI->IsInstanceSimulatingPhysics())
			{
				FVector ForcePoint = Impact.ImpactPoint;

				const float BodyMass = FMath::Max(BI->GetBodyMass(), 1.0f);

				if (bPushForceUsingZOffset)
				{
					FBox Bounds = BI->GetBodyBounds();

					FVector Center, Extents;
					Bounds.GetCenterAndExtents(Center, Extents);

					if (!Extents.IsNearlyZero())
					{
						ForcePoint.Z = Center.Z + Extents.Z * PushForcePointZOffsetFactor;
					}
				}

				FVector Force = Impact.ImpactNormal * -1.0f;

				float PushForceModificator = 1.0f;

				const FVector ComponentVelocity = ImpactComponent->GetPhysicsLinearVelocity();
				const FVector VirtualVelocity = ImpactAcceleration.IsZero() ? ImpactVelocity : ImpactAcceleration.GetSafeNormal() * GetMaxSpeed();

				float Dot = 0.0f;

				if (bScalePushForceToVelocity && !ComponentVelocity.IsNearlyZero())
				{
					Dot = ComponentVelocity | VirtualVelocity;

					if (Dot > 0.0f && Dot < 1.0f)
					{
						PushForceModificator *= Dot;
					}
				}

				if (bPushForceScaledToMass)
				{
					PushForceModificator *= BodyMass;
				}

				Force *= PushForceModificator;

				if (ComponentVelocity.IsNearlyZero())
				{
					Force *= InitialPushForceFactor;
					ImpactComponent->AddImpulseAtLocation(Force, ForcePoint, Impact.BoneName);
				}
				else
				{
					Force *= PushForceFactor;
					ImpactComponent->AddForceAtLocation(Force, ForcePoint, Impact.BoneName);
				}
			}
		}
	}
}

void UTD_PawnMovementComponent::PerformMovement(float DeltaTime)
{
	const FVector MoveVelocity = Velocity;

	const FVector Delta = FVector(MoveVelocity.X, MoveVelocity.Y, 0.f) * DeltaTime;
	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		float PercentTimeApplied = Hit.Time;
		HandleImpact(Hit, DeltaTime, Delta);
		SlideAlongSurface(Delta, 1.f - PercentTimeApplied, Hit.Normal, Hit, true);
	}
}
