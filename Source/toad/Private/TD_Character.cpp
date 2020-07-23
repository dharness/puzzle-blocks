#include "TD_Character.h"
#include "TD_Logging.h"
#include "Holdable.h"
#include "Kismet/KismetSystemLibrary.h"


ATD_Character::ATD_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	Holster2 = CreateDefaultSubobject<USceneComponent>(TEXT("Holster2"));
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

