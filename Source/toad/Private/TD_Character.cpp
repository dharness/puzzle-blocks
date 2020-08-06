#include "TD_Character.h"
#include "TD_Holdable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"


ATD_Character::ATD_Character()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATD_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHoldables();
}

void ATD_Character::Init(UPrimitiveComponent* NextGrabRegion)
{
	FActorSpawnParameters SpawnInfo;
	ATD_IKTarget* Actor = GetWorld()->SpawnActor<ATD_IKTarget>(ATD_IKTarget::StaticClass(), GetActorLocation(), GetActorRotation(), SpawnInfo);
	auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		false
	);
	Actor->AttachToActor(this, AttachmentRules);
	UE_LOG(LogTemp, Warning, TEXT("eggman %b"), IsValid(Actor));
	//AActor* HandIKTargetR = (AActor*)GetWorld()->SpawnActor(AActor::StaticClass(), FName(TEXT("HandIKTargetR")), GetActorLocation());

	GrabRegion = NextGrabRegion;

	GrabRegion->OnComponentBeginOverlap.AddDynamic(this, &ATD_Character::OnOverlapBegin);
	GrabRegion->OnComponentEndOverlap.AddDynamic(this, &ATD_Character::OnOverlapEnd);
}

void ATD_Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UpdateHoldables(OverlappedComp);
}

void ATD_Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UpdateHoldables(OverlappedComp);
	if (HasJustThrown && OtherActor == HeldObject)
	{
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(HeldObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		StaticMesh->SetCollisionProfileName(HeldObjectCollisionProfileName);
		HeldObject = nullptr;
		HasJustThrown = false;
	}
}

void ATD_Character::AttachToHolster(ATD_HoldableBase* ToGrab, USceneComponent* Holster, USceneComponent* RightHandle, bool bKeepHolsterLocation)
{
	if (IsValid(ToGrab))
	{
		auto AttachmentRules = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		);

		if (!bKeepHolsterLocation)
		{
			Holster->SetWorldLocation(ToGrab->GetActorLocation());
		}
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(ToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		StaticMesh->SetSimulatePhysics(false);
		HeldObjectCollisionProfileName = StaticMesh->GetCollisionProfileName();
		StaticMesh->SetCollisionProfileName(FName(TEXT("HeldObject")));
		ToGrab->AttachToComponent(Holster, AttachmentRules);
		HeldObject = ToGrab;
	}
}

void ATD_Character::Throw(float Strength)
{
	if (!IsValid(HeldObject)) return;

	UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(HeldObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	StaticMesh->SetSimulatePhysics(true);
	auto DetchmentRules = FDetachmentTransformRules(
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		false
	);
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(HeldObject->GetRootComponent());
	FVector ThrowVector = GetActorForwardVector() * Strength;
	PrimitiveComponent->SetPhysicsLinearVelocity(ThrowVector, true);
	HeldObject->GetRootComponent()->DetachFromComponent(DetchmentRules);
	HasJustThrown = true;
}

void ATD_Character::GetHoldable(bool& Success, ATD_HoldableBase*& Holdable)
{
	Success = CurrentHoldable != nullptr;
	Holdable = CurrentHoldable;
}

void ATD_Character::GetHeldObject(bool& Success, ATD_HoldableBase*& _HeldObject)
{
	Success = HeldObject != nullptr;
	_HeldObject = HeldObject;
}

void ATD_Character::UpdateHoldables()
{
	auto NextInteractable = Holdables.IsValidIndex(0) ? Holdables[0] : nullptr;

	if (NextInteractable != CurrentHoldable)
	{
		CurrentHoldable = NextInteractable;
		OnInteractableChanged();
	}
}

void ATD_Character::UpdateHoldables(UPrimitiveComponent* OverlappedComp)
{
	TArray<AActor*> OverlappingActors;
	OverlappedComp->GetOverlappingActors(OverlappingActors);

	TArray<ATD_HoldableBase*> NextHoldables;
	for (auto* Actor : OverlappingActors)
	{
		ATD_HoldableBase* Holdable = Cast<ATD_HoldableBase>(Actor);
		if (IsValid(Holdable))
		{
			NextHoldables.Emplace(Holdable);
		}
	}
	Holdables = NextHoldables;
}

// deprecated?
bool ATD_Character::IsInteractable(AActor* Actor)
{
	return UKismetSystemLibrary::DoesImplementInterface(Actor, UTD_Holdable::StaticClass());
}
