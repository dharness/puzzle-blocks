#include "TD_PropHidingSpot.h"


ATD_PropHidingSpot::ATD_PropHidingSpot()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
}

ETD_ActionTypes ATD_PropHidingSpot::GetInteractionType_Implementation()
{
	return ETD_ActionTypes::Hide;
}

