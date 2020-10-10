#include "TD_Character.h"

#include "DrawDebugHelpers.h"
#include "TD_Interactable.h"
#include "Components/SkeletalMeshComponent.h"
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
	UpdateCurrentActionOption();
}

void ATD_Character::Init(UPrimitiveComponent* NextGrabRegion, USceneComponent* NextHandsMesh, USceneComponent* NextEyes)
{
	InteractionRegion = NextGrabRegion;
	HandsMesh = NextHandsMesh;
	EyesMesh = NextEyes;
	CurrentState = ETD_CharacterStates::Idle;
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

void ATD_Character::HideUnder(AActor* ObjectToHideUnder)
{
	if (IsValid(ObjectToHideUnder))
	{
		USkeletalMeshComponent* StaticMesh = Cast<USkeletalMeshComponent>(ObjectToHideUnder->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		StaticMesh->SetSimulatePhysics(false);
		CurrentObjectCollisionProfileName = StaticMesh->GetCollisionProfileName();
		StaticMesh->SetCollisionProfileName(FName(TEXT("HeldObject")));
		CurrentObject = ObjectToHideUnder;
		this->bIsHiding = true;
	}
}

void ATD_Character::Unhide()
{
	if (IsValid(CurrentObject))
	{
		USkeletalMeshComponent* StaticMesh = Cast<USkeletalMeshComponent>(CurrentObject->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		StaticMesh->SetSimulatePhysics(true);
		StaticMesh->SetCollisionProfileName(CurrentObjectCollisionProfileName);
		CurrentObject = nullptr;
		this->bIsHiding = false;
	}
}

void ATD_Character::OnHideContactObject()
{
	auto AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false
	);

	FName SocketName = FName("Holster_2handsSocket");
	CurrentObject->AttachToComponent(HandsMesh, AttachmentRules, SocketName);
	CurrentState = ETD_CharacterStates::Hiding;
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

void ATD_Character::GetCurrentObject(bool& Success, AActor*& OutCurrentObject)
{
	if(CurrentObject != nullptr)
	{
		Success = true;
		OutCurrentObject = CurrentObject;
	}
}

void ATD_Character::UpdateCurrentActionOption()
{
	TArray<AActor*> OverlappingActors;
	InteractionRegion->GetOverlappingActors(OverlappingActors);
	float MinSquaredDistance = TNumericLimits< float >::Max();
	ITD_Interactable* ClosestInteractable = nullptr;

	// Check if we can take an action without interacting with any world objects
	FTD_CharacterAction* ActionOption = CheckDefaultActionOptions();
	if (ActionOption != nullptr)
	{
		CurrentActionOption = *ActionOption;
		return;
	}

	CurrentActionOption = FTD_CharacterAction();
	

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
					auto InteractionType = ClosestInteractable->Execute_GetInteractionType(Actor);
					if (!CanTakeAction(InteractionType)) { continue; }

					MinSquaredDistance = SquaredDistance;
					ClosestInteractable = Interactable;
					CurrentActionOption = FTD_CharacterAction();
					CurrentActionOption.InteractionType = ClosestInteractable->Execute_GetInteractionType(Actor);
					CurrentActionOption.Interactable = Actor;
				}
			}
		}
	}
}

FTD_CharacterAction* ATD_Character::CheckDefaultActionOptions()
{
	FTD_CharacterAction* ActionOption = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("HERE"));
	if (CurrentState == ETD_CharacterStates::Hiding && CurrentObject != nullptr)
	{
		ActionOption = new FTD_CharacterAction();
		ActionOption->Interactable = CurrentObject;
		ActionOption->InteractionType = ETD_ActionTypes::Unhide;
	}

	return ActionOption;
}

bool ATD_Character::CanTakeAction(ETD_ActionTypes InteractionType)
{
	if (InteractionType == ETD_ActionTypes::Grab) { return false; }
	
	return true;
}