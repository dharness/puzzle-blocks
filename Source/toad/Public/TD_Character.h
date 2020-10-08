// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TD_CharacterAction.h"
#include "TD_CharacterStates.h"
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
	bool bIsHiding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTD_CharacterAction CurrentActionOption;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETD_CharacterStates> CurrentState;

	UFUNCTION(BlueprintCallable)
	void Init(UPrimitiveComponent* GrabRegion, USceneComponent* HandsMesh, USceneComponent* Eyes);

	UFUNCTION(BlueprintCallable)
	void Throw(float Strength);

	UFUNCTION(BlueprintCallable)
	void Grab(ATD_InteractableBase* ToGrab);

	UFUNCTION(BlueprintCallable)
	void HideUnder(AActor* ObjectToHideUnder);

	UFUNCTION(BlueprintCallable)
	void OnHideContactObject();

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
	AActor* CurrentObject;
	UPROPERTY()
	FName HeldObjectCollisionProfileName;
	UPROPERTY()
	FName CurrentObjectCollisionProfileName;
	UPROPERTY()
	bool HasJustThrown;
	
	void UpdateCurrentActionOption();
	bool CanTakeAction(ETD_InteractionTypes InteractionType);
};
