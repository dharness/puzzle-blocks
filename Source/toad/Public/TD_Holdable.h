// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/Interface.h"
#include "TD_Holdable.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UTD_Holdable : public UInterface { GENERATED_BODY() };

class TOAD_API ITD_Holdable
{
	GENERATED_BODY()

public:
	USceneComponent* GetRightHandle();
};