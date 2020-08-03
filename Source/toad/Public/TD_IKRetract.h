// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_IKAction.h"
#include "TD_IKRetract.generated.h"

/**
 * 
 */
UCLASS()
class TOAD_API UTD_IKRetract : public UTD_IKAction
{
	GENERATED_BODY()

public:
	void OnActionTick() override;
	void OnActionEnd() override;

public:
	FString Tick(float DeltaTime);
};
