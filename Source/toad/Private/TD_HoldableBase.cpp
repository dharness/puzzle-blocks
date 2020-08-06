#include "TD_HoldableBase.h"
#include "Engine/StaticMesh.h" 


USceneComponent* ATD_HoldableBase::GetRightHandle_Implementation()
{
	return RightHandle;
}

float ATD_HoldableBase::GetHeight_Implementation()
{
	if (!IsValid(Mesh)) { return 0; }

	return Mesh->GetStaticMesh()->GetBounds().BoxExtent.Z;
}
