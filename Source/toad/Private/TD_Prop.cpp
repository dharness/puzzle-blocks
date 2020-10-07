// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_Prop.h"

ETD_InteractionTypes ATD_Prop::GetInteractionType_Implementation()
{
	return ETD_InteractionTypes::None;
}

ATD_Prop::ATD_Prop()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
}
