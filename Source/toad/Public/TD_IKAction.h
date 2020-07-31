// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"

#include "TD_IKAction.generated.h"

/**
 * 
 */
UCLASS()
class TOAD_API UTD_IKAction : public UObject
{
	GENERATED_BODY()

public:
	UTD_IKAction();

	UPROPERTY()
	FString ActionName;

	UPROPERTY()
	FTimeline Timeline;

	UPROPERTY()
	UCurveFloat* Curve;

	UFUNCTION()
	void Play();

	UFUNCTION()
	void Tick(float DeltaTime);

	UFUNCTION()
	void Init(UCurveFloat* _Curve, void (*TickPtr)());

	UFUNCTION()
	void OnActionTick();

	UFUNCTION()
	void OnActionEnd();
	
	UPROPERTY
	typedef void (*TickPtr)(void);

};
