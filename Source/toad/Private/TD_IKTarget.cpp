// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_IKTarget.h"

// Sets default values
ATD_IKTarget::ATD_IKTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void ATD_IKTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATD_IKTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

