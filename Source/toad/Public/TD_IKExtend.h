// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_IKArgs.h"
#include "TD_IKAction.h"
#include "TD_DelayedAction.h"

class FTD_IKExtend : public FTD_DelayedAction
{
public:
	FTD_IKExtend(const FLatentActionInfo& LatentInfo, UCurveFloat* Curve, FVector From, FVector To, FTD_IKArgs* IKArgs);

	void UpdateOperation(FLatentResponse& Response) override;

private:
	FTD_IKArgs* IKArgs;

	UPROPERTY()
	FTimeline Timeline;

	UCurveFloat* Curve;
	FVector From;
	FVector To;
};
