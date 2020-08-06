#include "TD_IKTarget.h"


ATD_IKTarget::ATD_IKTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}