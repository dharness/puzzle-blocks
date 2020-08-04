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
	void Init(UPrimitiveComponent* GrabRegion);

	UFUNCTION(BlueprintCallable)
	void AttachToHolster(AActor* ToGrab, USceneComponent* Holster, bool bKeepHolsterLocation);

	UFUNCTION(BlueprintCallable)
	void Throw(float Strength);

	UFUNCTION(BlueprintCallable, Category = "TD_Interaction")
	void GetInteractable(bool& Success, AActor*& Interactable);

	UFUNCTION(BlueprintCallable, Category = "TD_Interaction")
	void GetHeldObject(bool& Success, AActor*& HeldObject);

	UFUNCTION(BlueprintImplementableEvent, Category = "TD_Interaction")
	void OnInteractableChanged();

private:
	UPROPERTY()
	TArray<AActor*> InteractableActors;
	UPROPERTY()
	UPrimitiveComponent* GrabRegion;
	UPROPERTY()
	AActor* CurrentInteractable;
	UPROPERTY()
	AActor* HeldObject;
	UPROPERTY()
	FName HeldObjectCollisionProfileName;
	UPROPERTY()
	bool HasJustThrown;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void UpdateInteractables(UPrimitiveComponent* OverlappedComp);
	void UpdateInteractable();
	bool IsInteractable(AActor* Actor);
};
