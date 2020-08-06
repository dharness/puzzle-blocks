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

void UTD_HandIKComponent::Extend(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UCurveFloat* GrabCurve, USceneComponent* RightHandle)
{
	auto StartLocation = HandIKTargetR->GetComponentLocation();
	auto EndLocation = RightHandle->GetComponentLocation();

	FTD_IKExtend* IKExtend = new FTD_IKExtend(LatentInfo, GrabCurve, StartLocation, EndLocation, IKArgs);
	DelayedFunction<FTD_IKExtend>(WorldContextObject, LatentInfo, IKExtend);
}

void UTD_HandIKComponent::AttachIKToHandles(USceneComponent* RightHandle)
{
	auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		true
	);
	HandIKTargetR->AttachToComponent(RightHandle, AttachmentRules);
}

void UTD_HandIKComponent::RetractHolding(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UCurveFloat* Curve, ATD_HoldableBase* ToHold)
{
	auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		true
	);

	HolsterCenter->SetWorldLocation(ToHold->GetActorLocation());
	ToHold->GetRootComponent()->AttachToComponent(HolsterCenter, AttachmentRules);
	//ToHold->AttachToActor(this->GetOwner(), AttachmentRules, FName(TEXT("TestSocket")));
	//ToHold->AttachToComponent()
	return;

	auto StartLocation = HolsterCenter->GetComponentLocation();
	auto EndLocation = GetOwner()->GetRootComponent()->GetComponentLocation();
	auto ToHoldHalfHeight = ToHold->GetHeight() * 0.5;
	EndLocation.SetComponentForAxis(EAxis::Z, EndLocation.Z + ToHoldHalfHeight);

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