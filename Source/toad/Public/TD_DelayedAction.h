// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"

class FTD_DelayedAction : public FPendingLatentAction
{

public:
	int32 count;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	virtual void UpdateOperation(FLatentResponse& Response) override;

	/**
	* @param DialogComponent - Pointer to DialogComponent that called this function.
	* @param LatentInfo - latent info for callback
	*/
	FTD_DelayedAction(const FLatentActionInfo& LatentInfo);
};
