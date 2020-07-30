// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TD_HandAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOAD_API UTD_HandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform HandIKTargetR;
};
