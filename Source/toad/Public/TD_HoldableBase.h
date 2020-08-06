// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h" 
#include "TD_Holdable.h"
#include "TD_HoldableBase.generated.h"

UCLASS(Blueprintable)
class ATD_HoldableBase : public AActor, public ITD_Holdable
{
	GENERATED_BODY()

public:

	ATD_HoldableBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RightHandle;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USceneComponent* GetRightHandle();
	USceneComponent* GetRightHandle_Implementation();

	UFUNCTION(BlueprintCallable)
	float GetHeight();

	UFUNCTION(BlueprintCallable)
	void PrepareForHolding();
};
