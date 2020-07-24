// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "TD_Character.generated.h"

UCLASS(Blueprintable)
class TOAD_API ATD_Character : public APawn
{
	GENERATED_BODY()

public:
	ATD_Character();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Init(USceneComponent* Holster, UPrimitiveComponent* GrabRegion);

	UFUNCTION(BlueprintCallable)
	void Grab();

	UFUNCTION(BlueprintCallable)
	void Throw();


private:
	UPROPERTY()
	TArray<AActor*> InteractableActors;

	UPROPERTY()
	UPrimitiveComponent* GrabRegion;

	USceneComponent* Holster;

	AActor* CurrentInteractable;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateInteractables(UPrimitiveComponent* OverlappedComp);
	void UpdateInteractable();
	bool IsInteractable(AActor* Actor);
};
