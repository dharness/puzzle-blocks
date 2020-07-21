// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TD_PawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOAD_API UTD_PawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UTD_PawnMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** If enabled, the player will interact with physics objects when walking into them. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite)
	uint8 bEnablePhysicsInteraction : 1;

	/** If enabled, the TouchForceFactor is applied per kg mass of the affected object. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	uint8 bTouchForceScaledToMass : 1;

	/** If enabled, the PushForceFactor is applied per kg mass of the affected object. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	uint8 bPushForceScaledToMass : 1;

	/** If enabled, the PushForce location is moved using PushForcePointZOffsetFactor. Otherwise simply use the impact point. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	uint8 bPushForceUsingZOffset : 1;

	/** If enabled, the applied push force will try to get the physics object to the same velocity than the player, not faster. This will only
		scale the force down, it will never apply more force than defined by PushForceFactor. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	uint8 bScalePushForceToVelocity : 1;

	/** Force applied to objects we stand on (due to Mass and Gravity) is scaled by this amount. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float StandingDownwardForceScale;

	/** Initial impulse force to apply when the player bounces into a blocking physics object. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float InitialPushForceFactor;

	/** Force to apply when the player collides with a blocking physics object. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float PushForceFactor;

	/** Z-Offset for the position the force is applied to. 0.0f is the center of the physics object, 1.0f is the top and -1.0f is the bottom of the object. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (UIMin = "-1.0", UIMax = "1.0"), meta = (editcondition = "bEnablePhysicsInteraction"))
	float PushForcePointZOffsetFactor;

	/** Force to apply to physics objects that are touched by the player. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float TouchForceFactor;

	/** Minimum Force applied to touched physics objects. If < 0.0f, there is no minimum. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float MinTouchForce;

	/** Maximum force applied to touched physics objects. If < 0.0f, there is no maximum. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float MaxTouchForce;

	/** Force per kg applied constantly to all overlapping components. */
	UPROPERTY(Category = "Character Movement: Physics Interaction", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
	float RepulsionForce;

	/** Max Acceleration (rate of change of velocity) */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxAcceleration;

	/** Max Acceleration (rate of change of velocity) */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxDeceleration;

	/**
	* Current acceleration vector (with magnitude).
	* This is calculated each update based on the input vector and the constraints of MaxAcceleration and the current movement mode.
	*/
	UPROPERTY()
	FVector Acceleration;

	/**
	* Setting that affects movement control. Higher values allow faster changes in direction.
	* If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	* When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	* This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	* @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	*/
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float GroundFriction;

	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float BrakingFriction;

	UPROPERTY(Category = "Character Movement: General Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxSpeed;

	/**
	* Updates Velocity and Acceleration based on the current state, applying the effects of friction and acceleration or deceleration. Does not apply gravity.
	* This is used internally during movement updates. Normally you don't need to call this from outside code, but you might want to use it for custom movement modes.
	*
	* @param	DeltaTime						time elapsed since last frame.
	* @param	Friction						coefficient of friction when not accelerating, or in the direction opposite acceleration.
	* @param	bFluid							true if moving through a fluid, causing Friction to always be applied regardless of acceleration.
	* @param	BrakingDeceleration				deceleration applied when not accelerating, or when exceeding max velocity.
	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement")
	virtual void CalcVelocity(float DeltaTime, float Friction, FVector InputVector);

	void HandleImpact(const FHitResult& Impact, float TimeSlice, const FVector& MoveDelta);

	void ApplyImpactPhysicsForces(const FHitResult& Impact, const FVector& ImpactAcceleration, const FVector& ImpactVelocity);

	/** Stop completely when braking and velocity magnitude is lower than this. */
	static const float BRAKE_TO_STOP_VELOCITY;


	/**
	* Modifier to applied to values such as acceleration and max speed due to analog input.
	*/
	UPROPERTY()
	float AnalogInputModifier;

	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FVector ComputeTargetVelocity(const FVector& InputVector) const;
	FVector ComputeAcceleration(const FVector& InputVector) const;
	void PerformMovement(float DeltaTime);

private:
	FVector TargetVelocity;
};