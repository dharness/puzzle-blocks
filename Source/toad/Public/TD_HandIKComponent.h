// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "TD_IKArgs.h"
#include "TD_IKExtend.h"
#include "TD_IKMoveTo.h"
#include "TD_IKTarget.h"
#include "TD_HoldableBase.h"
#include "TD_HandAnimInstance.h"

#include "TD_HandIKComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOAD_API UTD_HandIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTD_HandIKComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Init(
		  UTD_HandAnimInstance* HandAnimInstance
		, USceneComponent* HolsterR
		, USceneComponent* HolsterL
		, USkeletalMeshComponent* HandR
		, USkeletalMeshComponent* HandL
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTD_HandAnimInstance* HandAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATD_IKTarget* HandIKTargetR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATD_IKTarget* HandIKTargetL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATD_IKTarget* HolsterCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* HolsterR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* HolsterL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HandR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HandL;

	UFUNCTION()
	void SyncAnimParams();
	
	UFUNCTION()
	ATD_IKTarget* MakeIKTarget(FName Name) const;

	FTD_IKArgs* IKArgs;

	UFUNCTION(BlueprintCallable, Category = "IKActions", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo"))
	void Extend(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UCurveFloat* GrabCurve, USceneComponent* RightHandle);

	UFUNCTION(BlueprintCallable, Category = "IKActions")
	void AttachIKToObject(ATD_HoldableBase* ToHold) const;

	UFUNCTION(BlueprintCallable, Category = "IKActions", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo"))
	void RetractHolding(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UCurveFloat* Curve, ATD_HoldableBase* ToHold);

	template<typename T>
	void DelayedFunction(UObject* WorldContextObject, FLatentActionInfo LatentInfo, T* LatentAction);
};
