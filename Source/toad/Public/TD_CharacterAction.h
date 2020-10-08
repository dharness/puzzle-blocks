// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_InteractionTypes.h"

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
	TEnumAsByte<ETD_InteractionTypes> InteractionType = ETD_InteractionTypes::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* Interactable = nullptr;
};