// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "TD_Prop.h"
#include "TD_PropSimple.generated.h"

UCLASS(Blueprintable)
class TOAD_API ATD_PropSimple : public ATD_Prop
{
	GENERATED_BODY()

// Interface functions
public:
	ETD_InteractionTypes GetInteractionType_Implementation() override;
};
