#include "TD_HandIKComponent.h"


UTD_HandIKComponent::UTD_HandIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTD_HandIKComponent::Init(
	  UTD_HandAnimInstance* _HandAnimInstance
	, USceneComponent* _HolsterR
	, USceneComponent* _HolsterL
	, USkeletalMeshComponent* _HandR
	, USkeletalMeshComponent* _HandL
)
{
	HandAnimInstance = _HandAnimInstance;
	HolsterR = _HolsterR;
	HolsterL = _HolsterL;
	HandR = _HandR;
	HandL = _HandL;


	HandIKTargetR = MakeIKTarget(FName(TEXT("HandIKTargetR")));
	HolsterCenter = MakeIKTarget(FName(TEXT("HolsterCenter")));

	IKArgs = new FTD_IKArgs();
	IKArgs->HandIKTargetR = HandIKTargetR;

	SyncAnimParams();
}

ATD_IKTarget* UTD_HandIKComponent::MakeIKTarget(const FName Name) const
{
	const FActorSpawnParameters SpawnInfo;

	ATD_IKTarget* IKTarget = GetWorld()->SpawnActor<ATD_IKTarget>(SpawnInfo);
	IKTarget->SetActorLabel(Name.ToString());

	const auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		false
	);
	IKTarget->AttachToActor(GetOwner(), AttachmentRules);

	return IKTarget;
}

void UTD_HandIKComponent::Extend(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UCurveFloat* GrabCurve, USceneComponent* RightHandle)
{
	const auto StartLocation = HandIKTargetR->GetActorLocation();
	const auto EndLocation = RightHandle->GetComponentLocation();

	FTD_IKExtend* IKExtend = new FTD_IKExtend(LatentInfo, GrabCurve, StartLocation, EndLocation, IKArgs);
	DelayedFunction<FTD_IKExtend>(WorldContextObject, LatentInfo, IKExtend);
}

void UTD_HandIKComponent::AttachIKToObject(ATD_HoldableBase* ToHold) const
{
	const auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		false
	);
	ToHold->PrepareForHolding();
	HandIKTargetR->AttachToActor(ToHold, AttachmentRules);
}

void UTD_HandIKComponent::RetractHolding(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UCurveFloat* Curve, ATD_HoldableBase* ToHold)
{
	const auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		false
	);

	HolsterCenter->SetActorLocation(ToHold->GetActorLocation());
	ToHold->AttachToActor(HolsterCenter, AttachmentRules);

	auto StartLocation = HolsterCenter->GetActorLocation();
	auto EndLocation = GetOwner()->GetActorLocation();
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
	FVector relativeLoc = HandR->GetComponentTransform().InverseTransformPosition(HandIKTargetR->GetActorLocation());
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