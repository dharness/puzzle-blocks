// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_ActionTypes.h"
#include "UObject/Interface.h"
#include "TD_Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTD_Interactable : public UInterface { GENERATED_BODY() };

class TOAD_API ITD_Interactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	ETD_ActionTypes GetInteractionType();
};