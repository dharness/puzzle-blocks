// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "TD_Interactable.h"
#include "TD_Prop.generated.h"

UCLASS(Blueprintable)
class TOAD_API ATD_Prop : public AActor, public ITD_Interactable
{
	GENERATED_BODY()

// Interface functions
public:
	ETD_InteractionTypes GetInteractionType_Implementation() override;

public:
	ATD_Prop();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

};
