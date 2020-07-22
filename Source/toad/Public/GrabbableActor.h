// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UsableActor.h"
#include "GrabbableActor.generated.h"

/**
 * 
 */
UCLASS()
class TOAD_API AGrabbableActor : public AUsableActor
{
	GENERATED_BODY()

	AGrabbableActor(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMesh;
	
};
