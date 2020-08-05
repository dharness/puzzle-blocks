// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TD_Holdable.generated.h"


UINTERFACE(MinimalAPI)
class UTD_Holdable : public UInterface
{
	GENERATED_BODY()

	FVector GetRightHandleLocation();
};

class TOAD_API ITD_Holdable
{
	GENERATED_BODY()

	UFUNCTION()
	virtual FVector GetRightHandleLocation();
};
