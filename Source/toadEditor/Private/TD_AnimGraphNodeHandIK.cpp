// Fill out your copyright notice in the Description page of Project Settings.

#include "TD_AnimGraphNodeHandIK.h"
#include "toadEditor.h"

UTD_AnimGraphNodeHandIK::UTD_AnimGraphNodeHandIK(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

FText UTD_AnimGraphNodeHandIK::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FromString(FString("Start Combo"));
}

FText UTD_AnimGraphNodeHandIK::GetTooltipText() const
{
    return FText::FromString(FString("Starts a combo"));
}

FString UTD_AnimGraphNodeHandIK::GetNodeCategory() const
{
    return FString("Combo");
}

FLinearColor UTD_AnimGraphNodeHandIK::GetNodeTitleColor() const
{
    return FLinearColor::Green;
}

FText UTD_AnimGraphNodeHandIK::GetControllerDescription() const
{
    return FText::FromString(FString("Controller Description"));
}