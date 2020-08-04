// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_IKArgs.h"
#include "TD_IKAction.h"
#include "TD_DelayedAction.h"

class FTD_IKMoveTo : public FTD_DelayedAction
{
public:
	FTD_IKMoveTo(const FLatentActionInfo& LatentInfo, UCurveFloat* Curve, FVector From, FVector To, USceneComponent* Target);

	void UpdateOperation(FLatentResponse& Response) override;

private:
	USceneComponent* Target;
	FTimeline Timeline;
	UCurveFloat* Curve;
	FVector From;
	FVector To;
};
