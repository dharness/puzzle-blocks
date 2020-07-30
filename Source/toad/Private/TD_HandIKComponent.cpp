#include "TD_HandIKComponent.h"
#include "Math/UnrealMathVectorCommon.h"

UTD_HandIKComponent::UTD_HandIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTD_HandIKComponent::Init(
	UTD_HandAnimInstance* _HandAnimInstance,
	UCurveFloat* _GrabCurve,
	USceneComponent* _HandIKTargetR)
{
	HandAnimInstance = _HandAnimInstance;
	GrabCurve = _GrabCurve;
	HandIKTargetR = _HandIKTargetR;

	BindTimelineFunctions();
	SyncAnimParams();
}


void UTD_HandIKComponent::BindTimelineFunctions()
{
	if (GrabCurve)
	{
		UE_LOG(LogTemp, Log, TEXT("Grab curve set"));
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("OnTimelineTick"));
		TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("OnTimelineEnd") });
		GrabTimeline.AddInterpFloat(GrabCurve, TimelineCallback);
		GrabTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

void UTD_HandIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GrabTimeline.TickTimeline(DeltaTime);
}

void UTD_HandIKComponent::Grab(USceneComponent* ToGrab, USceneComponent* GrabAnchor)
{
	StartLocation = GrabAnchor->GetComponentLocation();
	EndLocation = ToGrab->GetComponentLocation();
	GrabTimeline.PlayFromStart();
}

void UTD_HandIKComponent::OnTimelineTick()
{
	float TimelineValue = GrabTimeline.GetPlaybackPosition();
	float Percent = GrabCurve->GetFloatValue(TimelineValue);
	FVector NextLocation = FMath::Lerp(StartLocation, EndLocation, Percent);
	HandIKTargetR->SetWorldLocation(NextLocation);
	SyncAnimParams();
}

void UTD_HandIKComponent::OnTimelineEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("END!!"));
}

void UTD_HandIKComponent::SyncAnimParams()
{
	// We use relative location because the FABRIK node in the anim controller expects it
	HandAnimInstance->HandIKTargetR.SetLocation(HandIKTargetR->GetRelativeLocation());
}