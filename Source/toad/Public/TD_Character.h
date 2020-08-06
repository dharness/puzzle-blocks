// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "TD_HoldableBase.h"
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
	void AttachToHolster(ATD_HoldableBase* ToGrab, USceneComponent* Holster, USceneComponent* RightHandle, bool bKeepHolsterLocation);

	UFUNCTION(BlueprintCallable)
	void Throw(float Strength);

	UFUNCTION(BlueprintCallable, Category = "TD_Interaction")
	void GetHoldable(bool& Success, ATD_HoldableBase*& Holdable);

	UFUNCTION(BlueprintCallable, Category = "TD_Interaction")
	void GetHeldObject(bool& Success, ATD_HoldableBase*& HeldObject);

	UFUNCTION(BlueprintImplementableEvent, Category = "TD_Interaction")
	void OnInteractableChanged();

private:
	UPROPERTY()
	TArray<ATD_HoldableBase*> Holdables;
	UPROPERTY()
	UPrimitiveComponent* GrabRegion;
	UPROPERTY()
	ATD_HoldableBase* CurrentHoldable;
	UPROPERTY()
	ATD_HoldableBase* HeldObject;
	UPROPERTY()
	FName HeldObjectCollisionProfileName;
	UPROPERTY()
	bool HasJustThrown;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void UpdateHoldables(UPrimitiveComponent* OverlappedComp);
	void UpdateHoldables();
	bool IsInteractable(AActor* Actor);
};
