// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "TD_Character.generated.h"

UCLASS()
class TOAD_API ATD_Character : public APawn
{
	GENERATED_BODY()

public:
	ATD_Character();

	UPROPERTY(BlueprintReadWrite)
	USceneComponent* Holster2;

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void Grab();

	UFUNCTION(BlueprintCallable)
	void Throw();

private:
	TArray<AActor*> InteractableActors;
	
	void UpdateInteractables(UPrimitiveComponent* OverlappedComp);
	bool IsInteractable(AActor* Actor);
};
