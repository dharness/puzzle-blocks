#include "TD_HandIKComponent.h"


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
	IKArgs = new FTD_IKArgs();
	IKArgs->HandIKTargetR = HandIKTargetR;

	SyncAnimParams();
}

void UTD_HandIKComponent::Extend(UObject* WorldContextObject, USceneComponent* ToGrab, struct FLatentActionInfo LatentInfo)
{
	auto StartLocation = HandIKTargetR->GetComponentLocation();
	auto EndLocation = ToGrab->GetComponentLocation();
	FTD_IKExtend* IKExtend = new FTD_IKExtend(LatentInfo, GrabCurve, StartLocation, EndLocation, IKArgs);
	DelayedFunction<FTD_IKExtend>(WorldContextObject, LatentInfo, IKExtend);
}

void UTD_HandIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SyncAnimParams();
}

void UTD_HandIKComponent::SyncAnimParams()
{
	// We use relative location because the FABRIK node in the anim controller expects it
	HandAnimInstance->HandIKTargetR.SetLocation(HandIKTargetR->GetRelativeLocation());
}

template<typename T>
void UTD_HandIKComponent::DelayedFunction(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, T* LatentAction)

{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<T>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, LatentAction);
		}
	}
}