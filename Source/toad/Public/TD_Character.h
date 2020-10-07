// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_CharacterAction.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "TD_InteractableBase.h"
#include "TD_Character.generated.h"

UCLASS(Blueprintable)
class TOAD_API ATD_Character : public APawn
{
	GENERATED_BODY()

public:
	ATD_Character();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTD_CharacterAction CurrentAction;

	UFUNCTION(BlueprintCallable)
	void Init(UPrimitiveComponent* GrabRegion, USceneComponent* HandsMesh, USceneComponent* Eyes);

	UFUNCTION(BlueprintCallable)
	void Throw(float Strength);

	UFUNCTION(BlueprintCallable)
	void Grab(ATD_InteractableBase* ToGrab);

	UFUNCTION(BlueprintCallable, Category = "TD_Interaction")
	void GetHoldable(bool& Success, ATD_InteractableBase*& Holdable);

	UFUNCTION(BlueprintCallable, Category = "TD_Interaction")
	void GetHeldObject(bool& Success, ATD_InteractableBase*& HeldObject);

	UFUNCTION(BlueprintImplementableEvent, Category = "TD_Interaction")
	void OnInteractableChanged();

private:
	UPROPERTY()
	TArray<ATD_InteractableBase*> Interactables;
	UPROPERTY()
	UPrimitiveComponent* InteractionRegion;
	UPROPERTY()
	USceneComponent* HandsMesh;
	UPROPERTY()
	USceneComponent* EyesMesh;
	UPROPERTY()
	ATD_InteractableBase* CurrentInteractable;
	UPROPERTY()
	ATD_InteractableBase* HeldObject;
	UPROPERTY()
	FName HeldObjectCollisionProfileName;
	UPROPERTY()
	bool HasJustThrown;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void UpdateCurrentAction();
	bool IsInteractable(AActor* Actor);
};
