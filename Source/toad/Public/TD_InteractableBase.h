// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h" 
#include "TD_Interactable.h"
#include "TD_InteractableBase.generated.h"

UCLASS(Blueprintable)
class ATD_InteractableBase : public AActor, public ITD_Interactable
{
	GENERATED_BODY()

public:

	ATD_InteractableBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InteractionDisplayName;

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
