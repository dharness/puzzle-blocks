#include "GrabbableActor.h"

AGrabbableActor::AGrabbableActor(const FObjectInitializer& ObjectInitializer)
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static mesh"));
}