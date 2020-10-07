#include "TD_Character.h"

#include "DrawDebugHelpers.h"
#include "TD_Interactable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"


ATD_Character::ATD_Character()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATD_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCurrentAction();
}

void ATD_Character::Init(UPrimitiveComponent* NextGrabRegion, USceneComponent* NextHandsMesh, USceneComponent* NextEyes)
{
	InteractionRegion = NextGrabRegion;
	HandsMesh = NextHandsMesh;
	EyesMesh = NextEyes;

	InteractionRegion->OnComponentBeginOverlap.AddDynamic(this, &ATD_Character::OnOverlapBegin);
	InteractionRegion->OnComponentEndOverlap.AddDynamic(this, &ATD_Character::OnOverlapEnd);
}

void ATD_Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ITD_Interactable* InteractableObject = Cast<ITD_Interactable>(OtherActor);
	//if (InteractableObject != nullptr)
	//{
	//	const auto InteractionType = InteractableObject->Execute_GetInteractionType(OtherActor);
	//	CurrentAction = FTD_CharacterAction();
	//	CurrentAction.InteractionType = InteractionType;
	//}
	UpdateCurrentAction();
}

void ATD_Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UpdateCurrentAction();
	//if (HasJustThrown && OtherActor == HeldObject)
	//{
	//	UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(HeldObject->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	//	StaticMesh->SetCollisionProfileName(HeldObjectCollisionProfileName);
	//	HeldObject = nullptr;
	//	HasJustThrown = false;
	//}
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

void ATD_Character::Grab(ATD_InteractableBase* ToGrab)
{
	if (IsValid(ToGrab))
	{
		auto AttachmentRules = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		);

		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(ToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		StaticMesh->SetSimulatePhysics(false);
		HeldObjectCollisionProfileName = StaticMesh->GetCollisionProfileName();
		StaticMesh->SetCollisionProfileName(FName(TEXT("HeldObject")));
		FName SocketName = FName("Holster_2handsSocket");
		ToGrab->AttachToComponent(HandsMesh, AttachmentRules, SocketName);
		HeldObject = ToGrab;
	}
}

void ATD_Character::GetHoldable(bool& Success, ATD_InteractableBase*& Holdable)
{
	Success = CurrentInteractable != nullptr;
	Holdable = CurrentInteractable;
}

void ATD_Character::GetHeldObject(bool& Success, ATD_InteractableBase*& _HeldObject)
{
	Success = HeldObject != nullptr;
	_HeldObject = HeldObject;
}

void ATD_Character::UpdateCurrentAction()
{
	TArray<AActor*> OverlappingActors;
	InteractionRegion->GetOverlappingActors(OverlappingActors);
	float MinSquaredDistance = TNumericLimits< float >::Max();
	ITD_Interactable* ClosestInteractable = nullptr;

	CurrentAction = FTD_CharacterAction();
	CurrentAction.InteractionType = ETD_InteractionTypes::None;

	for (auto* Actor : OverlappingActors)
	{
		ITD_Interactable* Interactable = Cast<ITD_Interactable>(Actor);
		if (Interactable != nullptr)
		{
			FHitResult OutHit;
			FVector Start = EyesMesh->GetComponentLocation();
			FVector End = Actor->GetActorLocation();
			FCollisionQueryParams CollisionParams;

			bool HasHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);
			if (HasHit)
			{
				float SquaredDistance = (OutHit.Location - Start).SizeSquared();
				if (SquaredDistance < MinSquaredDistance)
				{
					MinSquaredDistance = SquaredDistance;
					ClosestInteractable = Interactable;
					CurrentAction = FTD_CharacterAction();
					CurrentAction.InteractionType = ClosestInteractable->Execute_GetInteractionType(Actor);
				}
			}
		}
	}
}

// deprecated?
bool ATD_Character::IsInteractable(AActor* Actor)
{
	return UKismetSystemLibrary::DoesImplementInterface(Actor, UTD_Interactable::StaticClass());
}
