// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_DelayedAction.h"

FTD_DelayedAction::FTD_DelayedAction(const FLatentActionInfo& LatentInfo):
	ExecutionFunction(LatentInfo.ExecutionFunction)
	, OutputLink(LatentInfo.Linkage)
	, CallbackTarget(LatentInfo.CallbackTarget)
{
	count = 0;
	UE_LOG(LogTemp, Warning, TEXT("Creating"));
}

void FTD_DelayedAction::UpdateOperation(FLatentResponse& Response)
{
	count++;
	Response.FinishAndTriggerIf(count >= 300, ExecutionFunction, OutputLink, CallbackTarget);
}
