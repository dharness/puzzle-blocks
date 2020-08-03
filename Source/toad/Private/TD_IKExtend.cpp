#include "TD_IKExtend.h"
#include "Math/UnrealMathVectorCommon.h"

FTD_IKExtend::FTD_IKExtend(const FLatentActionInfo& LatentInfo, UCurveFloat* Curve, FVector From, FVector To, FTD_IKArgs* IKArgs) :
	FTD_DelayedAction(LatentInfo)
	, Curve(Curve)
	, From(From)
	, To(To)
	, IKArgs(IKArgs)
{
	Timeline = FTimeline{};

	FOnTimelineFloat Noop;
	Timeline.AddInterpFloat(Curve, Noop);
	Timeline.PlayFromStart();
};

void FTD_IKExtend::UpdateOperation(FLatentResponse& Response)
{
	bool bIsFinished = Timeline.GetTimelineLength() == Timeline.GetPlaybackPosition();
	Response.FinishAndTriggerIf(bIsFinished, ExecutionFunction, OutputLink, CallbackTarget);
	if (bIsFinished) { return;  }

	Timeline.TickTimeline(Response.ElapsedTime());
	float TimelineValue = Timeline.GetPlaybackPosition();
	float Percent = Curve->GetFloatValue(TimelineValue);
	FVector NextLocation = FMath::Lerp(From, To, Percent);
	IKArgs->HandIKTargetR->SetWorldLocation(NextLocation);
	IKArgs->TimelineValue = TimelineValue;
}