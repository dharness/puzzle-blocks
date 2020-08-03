#include "TD_IKRetract.h"
#include "Math/UnrealMathVectorCommon.h"

FString UTD_IKRetract::Tick(float DeltaTime)
{
	Timeline.TickTimeline(DeltaTime);
	float TimelineValue = Timeline.GetPlaybackPosition();
	float Percent = Curve->GetFloatValue(TimelineValue);
	UE_LOG(LogTemp, Warning, TEXT("Retract::Tick %f"), TimelineValue);

	return "success";
}

void UTD_IKRetract::OnActionTick()
{
	UE_LOG(LogTemp, Warning, TEXT("Retract End"));
}

void UTD_IKRetract::OnActionEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Retract End"));
}