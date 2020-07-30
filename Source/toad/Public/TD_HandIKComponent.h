// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"

#include "TD_HandAnimInstance.h"
#include "TD_HandIKComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOAD_API UTD_HandIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTD_HandIKComponent();

	UFUNCTION(BlueprintCallable)
	void Init(
		UTD_HandAnimInstance* HandAnimInstance,
		UCurveFloat* GrabCurve,
		USceneComponent* HandIKTargetR
	);

public:	
	void BindTimelineFunctions();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTD_HandAnimInstance* HandAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCurveFloat* GrabCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* HandIKTargetR;

	UFUNCTION(BlueprintCallable)
		void Grab(USceneComponent* ToGrab, USceneComponent* GrabAnchor);

	UFUNCTION()
		void OnTimelineTick();

	UFUNCTION()
		void OnTimelineEnd();

	void SyncAnimParams();

	FTimeline GrabTimeline;
	FVector StartLocation;
	FVector EndLocation;
		
};
