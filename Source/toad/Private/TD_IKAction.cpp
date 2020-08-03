#include "TD_IKAction.h"


void UTD_IKAction::Init(UCurveFloat* _Curve)
{
	Curve = _Curve;
	Timeline = FTimeline{};

	FOnTimelineFloat TickCallback;
	FOnTimelineEventStatic FinishedCallback;

	TickCallback.BindUFunction(this, FName("OnActionTick"));
	FinishedCallback.BindUFunction(this, FName{ TEXT("OnActionEnd") });
	Timeline.AddInterpFloat(Curve, TickCallback);
	Timeline.SetTimelineFinishedFunc(FinishedCallback);
}

void UTD_IKAction::Play()
{
	Timeline.PlayFromStart();
}

FString UTD_IKAction::Tick(float DeltaTime)
{
	Timeline.TickTimeline(DeltaTime);
	return FString("Lemmonadse");
}

void UTD_IKAction::OnActionTick()
{
	UE_LOG(LogTemp, Warning, TEXT("Action Tick"));
}

void UTD_IKAction::OnActionEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Action End"));
}
