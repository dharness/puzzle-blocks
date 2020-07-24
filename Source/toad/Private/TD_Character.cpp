#include "TD_Character.h"
#include "TD_Logging.h"
#include "Holdable.h"
#include "Kismet/KismetSystemLibrary.h"


ATD_Character::ATD_Character()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATD_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (InteractableActors.Num() > 0)
	{
		UpdateInteractable();
	}
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
}

void ATD_Character::Grab()
{
	TD_Logging::LogDefault("Grabbing");
}

void ATD_Character::Throw()
{
}

void ATD_Character::UpdateInteractable()
{
	if (InteractableActors.IsValidIndex(0))
	{
		CurrentInteractable = InteractableActors[0];
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
	TD_Logging::LogDefault(InteractableActors.Num());
}

bool ATD_Character::IsInteractable(AActor* Actor)
{
	return UKismetSystemLibrary::DoesImplementInterface(Actor, UHoldable::StaticClass());
}
