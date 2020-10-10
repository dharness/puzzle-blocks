#include "TD_PropHidingSpot.h"


ATD_PropHidingSpot::ATD_PropHidingSpot()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
}

ETD_InteractionTypes ATD_PropHidingSpot::GetInteractionType_Implementation()
{
	return ETD_InteractionTypes::HidingSpot;
}

