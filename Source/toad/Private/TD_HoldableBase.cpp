#include "TD_HoldableBase.h"
#include "Engine/StaticMesh.h" 


ATD_HoldableBase::ATD_HoldableBase()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = Mesh;

	RightHandle = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandle"));
	RightHandle->SetupAttachment(RootComponent);
	
}

USceneComponent* ATD_HoldableBase::GetRightHandle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("RightHandle: %s"), *RightHandle->GetRelativeLocation().ToString())
	return RightHandle;
}

float ATD_HoldableBase::GetHeight()
{
	if (!IsValid(Mesh)) { return 0; }

	return 30.0f;
	//return Mesh->GetStaticMesh()->GetBounds().BoxExtent.Z;
}

void ATD_HoldableBase::PrepareForHolding()
{
	if (!IsValid(Mesh)) { return;  }
	Mesh->SetSimulatePhysics(false);
}
