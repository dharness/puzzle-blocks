// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_ActionTypes.h"

#include "TD_CharacterAction.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTD_CharacterAction
{
    GENERATED_USTRUCT_BODY();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETD_ActionTypes> InteractionType = ETD_ActionTypes::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* Interactable = nullptr;
};