// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TD_Prop.h"
#include "TD_PropHidingSpot.generated.h"

UCLASS()
class TOAD_API ATD_PropHidingSpot : public ATD_Prop
{
	GENERATED_BODY()

// Interface functions
public:
	ETD_InteractionTypes GetInteractionType_Implementation() override;
};
