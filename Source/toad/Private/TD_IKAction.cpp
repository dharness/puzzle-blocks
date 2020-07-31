// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_IKAction.h"

UTD_IKAction::UTD_IKAction(){}

void UTD_IKAction::Init(UCurveFloat* _Curve, void (*_TickPtr)())
{
	Curve = _Curve;
	Timeline = FTimeline{};
	TickPtr = _TickPtr;

	// Extend
	FOnTimelineFloat TickCallback;
	FOnTimelineEventStatic FinishedCallback;

	TickCallback.BindUFunction(this, FName("OnActionTick"));
	FinishedCallback.BindUFunction(this, FName{ TEXT("OnActionEnd") });
	Timeline.AddInterpFloat(Curve, TickCallback);
	Timeline.SetTimelineFinishedFunc(FinishedCallback);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *(Curve->GetName()));
}

void UTD_IKAction::Play()
{
	Timeline.PlayFromStart();
}

void UTD_IKAction::Tick(float DeltaTime)
{
	Timeline.TickTimeline(DeltaTime);
	TickPtr();
}

void UTD_IKAction::OnActionTick()
{
	UE_LOG(LogTemp, Warning, TEXT("Action Tick"));
}

void UTD_IKAction::OnActionEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Action End"));
}
