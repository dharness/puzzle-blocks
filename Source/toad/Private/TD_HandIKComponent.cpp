#include "TD_HandIKComponent.h"


UTD_HandIKComponent::UTD_HandIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTD_HandIKComponent::Init(
	  UTD_HandAnimInstance* _HandAnimInstance
	, USceneComponent* _HandIKTargetR
	, USceneComponent* _HandIKTargetL
	, USceneComponent* _HolsterCenter
	, USceneComponent* _HolsterR
	, USceneComponent* _HolsterL
	, USkeletalMeshComponent* _HandR
	, USkeletalMeshComponent* _HandL
)
{
	HandAnimInstance = _HandAnimInstance;
	HandIKTargetR = _HandIKTargetR;
	HandIKTargetL = _HandIKTargetL;
	HolsterCenter = _HolsterCenter;
	HolsterR = _HolsterR;
	HolsterL = _HolsterL;
	HandR = _HandR;
	HandL = _HandL;

	IKArgs = new FTD_IKArgs();
	IKArgs->HandIKTargetR = HandIKTargetR;

	SyncAnimParams();
}

void UTD_HandIKComponent::Extend(UObject* WorldContextObject, USceneComponent* ToGrab, UCurveFloat* GrabCurve, struct FLatentActionInfo LatentInfo)
{
	auto StartLocation = HandIKTargetR->GetComponentLocation();
	auto EndLocation = ToGrab->GetComponentLocation();

	// Pick End Location based on object size
	// then make the IK the child of some handle object attached to the object
	// then anchor the object to the holster
	// then move the holster

	FTD_IKExtend* IKExtend = new FTD_IKExtend(LatentInfo, GrabCurve, StartLocation, EndLocation, IKArgs);
	DelayedFunction<FTD_IKExtend>(WorldContextObject, LatentInfo, IKExtend);
}

void UTD_HandIKComponent::RetractHolding(UObject* WorldContextObject, UCurveFloat* Curve, struct FLatentActionInfo LatentInfo)
{
	auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		false
	);
	HandIKTargetR->AttachToComponent(HolsterCenter, AttachmentRules);

	auto StartLocation = HolsterCenter->GetComponentLocation();
	auto EndLocation = GetOwner()->GetRootComponent()->GetComponentLocation();
	EndLocation.SetComponentForAxis(EAxis::Z, 100.0f);

	FTD_IKMoveTo* IKMoveTo = new FTD_IKMoveTo(LatentInfo, Curve, StartLocation, EndLocation, HolsterCenter);
	DelayedFunction<FTD_IKMoveTo>(WorldContextObject, LatentInfo, IKMoveTo);
}

void UTD_HandIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SyncAnimParams();
}

void UTD_HandIKComponent::SyncAnimParams()
{
	// We use relative location because the FABRIK node in the anim controller expects it
	FVector relativeLoc = HandR->GetComponentTransform().InverseTransformPosition(HandIKTargetR->GetComponentLocation());
	HandAnimInstance->HandIKTargetR.SetLocation(relativeLoc);
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