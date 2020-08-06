#include "TD_IKMoveTo.h"
#include "Math/UnrealMathVectorCommon.h"

FTD_IKMoveTo::FTD_IKMoveTo(const FLatentActionInfo& LatentInfo, UCurveFloat* Curve, FVector From, FVector To, AActor* Target) :
	FTD_DelayedAction(LatentInfo)
	, Curve(Curve)
	, From(From)
	, To(To)
	, Target(Target)
{
	Timeline = FTimeline{};

	FOnTimelineFloat Noop;
	Timeline.AddInterpFloat(Curve, Noop);
	Timeline.PlayFromStart();
};

void FTD_IKMoveTo::UpdateOperation(FLatentResponse& Response)
{
	bool bIsFinished = Timeline.GetTimelineLength() == Timeline.GetPlaybackPosition();
	Response.FinishAndTriggerIf(bIsFinished, ExecutionFunction, OutputLink, CallbackTarget);
	if (bIsFinished) { return;  }

	Timeline.TickTimeline(Response.ElapsedTime());
	float TimelineValue = Timeline.GetPlaybackPosition();
	float Percent = Curve->GetFloatValue(TimelineValue);
	FVector NextLocation = FMath::Lerp(From, To, Percent);
	Target->SetActorLocation(NextLocation);
}