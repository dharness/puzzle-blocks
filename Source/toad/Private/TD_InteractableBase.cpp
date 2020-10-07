#include "TD_InteractableBase.h"
#include "Engine/StaticMesh.h" 


ATD_InteractableBase::ATD_InteractableBase()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = Mesh;

	RightHandle = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandle"));
	RightHandle->SetupAttachment(RootComponent);
	
}

USceneComponent* ATD_InteractableBase::GetRightHandle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("RightHandle: %s"), *RightHandle->GetRelativeLocation().ToString())
	return RightHandle;
}

float ATD_InteractableBase::GetHeight()
{
	if (!IsValid(Mesh)) { return 0; }

	return 30.0f;
	//return Mesh->GetStaticMesh()->GetBounds().BoxExtent.Z;
}

void ATD_InteractableBase::PrepareForHolding()
{
	if (!IsValid(Mesh)) { return;  }
	Mesh->SetSimulatePhysics(false);
}
