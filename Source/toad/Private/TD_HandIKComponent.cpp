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

	IKAction = NewObject<UTD_IKAction>(UTD_IKAction::StaticClass());
	IKAction->Init(GrabCurve);
	IKAction->Play();
	//BindTimelineFunctions();
	SyncAnimParams();
}


void UTD_HandIKComponent::BindTimelineFunctions()
{
	if (GrabCurve)
	{
		// Extend
		FOnTimelineFloat ExtendCallback;
		FOnTimelineEventStatic ExtendFinishedCallback;
		
		ExtendCallback.BindUFunction(this, FName("OnExtendTick"));
		ExtendFinishedCallback.BindUFunction(this, FName{ TEXT("OnExtendEnd") });
		ExtendTimeline.AddInterpFloat(GrabCurve, ExtendCallback);
		ExtendTimeline.SetTimelineFinishedFunc(ExtendFinishedCallback);
		
		// Retract
		FOnTimelineFloat RetractCallback;
		FOnTimelineEventStatic RetractFinishedCallback;

		RetractCallback.BindUFunction(this, FName("OnRetractTick"));
		RetractFinishedCallback.BindUFunction(this, FName{ TEXT("OnRetractEnd") });
		RetractTimeline.AddInterpFloat(GrabCurve, RetractCallback);
		RetractTimeline.SetTimelineFinishedFunc(RetractFinishedCallback);
	}
}


void UTD_HandIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(IKAction))
	{
		IKAction->Tick(DeltaTime);
	}

	/*ExtendTimeline.TickTimeline(DeltaTime);
	RetractTimeline.TickTimeline(DeltaTime);*/
}

void UTD_HandIKComponent::Grab(USceneComponent* ToGrab)
{
	IKAction->Play();
	/*StartLocation = HandIKTargetR->GetComponentLocation();
	EndLocation = ToGrab->GetComponentLocation();
	ExtendTimeline.PlayFromStart();*/
}

void UTD_HandIKComponent::OnExtendTick()
{
	UE_LOG(LogTemp, Warning, TEXT("Extend Tick"));
	//float TimelineValue = ExtendTimeline.GetPlaybackPosition();
	//float Percent = GrabCurve->GetFloatValue(TimelineValue);
	//FVector NextLocation = FMath::Lerp(StartLocation, EndLocation, Percent);
	//HandIKTargetR->SetWorldLocation(NextLocation);
	//SyncAnimParams();
}

void UTD_HandIKComponent::OnExtendEnd()
{
	OnGrabContact.Broadcast();
	EndLocation = StartLocation;
	StartLocation = HandIKTargetR->GetComponentLocation();

	RetractTimeline.PlayFromStart();
}

void UTD_HandIKComponent::OnRetractTick()
{
	UE_LOG(LogTemp, Warning, TEXT("Retracting"));
	float TimelineValue = RetractTimeline.GetPlaybackPosition();
	float Percent = GrabCurve->GetFloatValue(TimelineValue);
	FVector NextLocation = FMath::Lerp(StartLocation, EndLocation, Percent);
	HandIKTargetR->SetWorldLocation(NextLocation);
	SyncAnimParams();
}

void UTD_HandIKComponent::OnRetractEnd() {
	UE_LOG(LogTemp, Warning, TEXT("END"));
}

void UTD_HandIKComponent::SyncAnimParams()
{
	// We use relative location because the FABRIK node in the anim controller expects it
	HandAnimInstance->HandIKTargetR.SetLocation(HandIKTargetR->GetRelativeLocation());
}