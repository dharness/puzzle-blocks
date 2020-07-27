#include "TD_Character.h"
#include "TD_Logging.h"
#include "Holdable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"


ATD_Character::ATD_Character()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATD_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractable();
}

void ATD_Character::Init(USceneComponent* NextHolster, UPrimitiveComponent* NextGrabRegion)
{
	Holster = NextHolster;
	GrabRegion = NextGrabRegion;
	TD_Logging::LogDefault(IsValid(GrabRegion));

	GrabRegion->OnComponentBeginOverlap.AddDynamic(this, &ATD_Character::OnOverlapBegin);
	GrabRegion->OnComponentEndOverlap.AddDynamic(this, &ATD_Character::OnOverlapEnd);
}

void ATD_Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UpdateInteractables(OverlappedComp);
}

void ATD_Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UpdateInteractables(OverlappedComp);
	if (HasJustThrown && OtherActor == HeldObject)
	{
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(HeldObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		StaticMesh->SetCollisionProfileName(HeldObjectCollisionProfileName);
		HeldObject = nullptr;
		HasJustThrown = false;
	}
}

void ATD_Character::Grab(AActor* ToGrab)
{
	if (IsValid(ToGrab))
	{
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(ToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		StaticMesh->SetSimulatePhysics(false);
		HeldObjectCollisionProfileName = StaticMesh->GetCollisionProfileName();
		StaticMesh->SetCollisionProfileName(FName(TEXT("HeldObject")));
		auto AttachmentRules = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		);
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

void ATD_Character::GetInteractable(bool& Success, AActor*& Interactable)
{
	Success = CurrentInteractable != nullptr;
	Interactable = CurrentInteractable;
}

void ATD_Character::GetHeldObject(bool& Success, AActor*& _HeldObject)
{
	Success = HeldObject != nullptr;
	_HeldObject = HeldObject;
}

void ATD_Character::UpdateInteractable()
{
	auto NextInteractable = InteractableActors.IsValidIndex(0) ? InteractableActors[0] : nullptr;

	if (NextInteractable != CurrentInteractable)
	{
		CurrentInteractable = NextInteractable;
		OnInteractableChanged();
	}
}

void ATD_Character::UpdateInteractables(UPrimitiveComponent* OverlappedComp)
{
	TArray<AActor*> OverlappingActors;
	OverlappedComp->GetOverlappingActors(OverlappingActors);

	TArray<AActor*> NextInteractableActors;
	for (auto* Actor : OverlappingActors)
	{
		bool IsInteractable = this->IsInteractable(Actor);
		if (IsInteractable)
		{
			NextInteractableActors.Emplace(Actor);
		}
	}
	InteractableActors = NextInteractableActors;
}

bool ATD_Character::IsInteractable(AActor* Actor)
{
	return UKismetSystemLibrary::DoesImplementInterface(Actor, UHoldable::StaticClass());
}
