// Copyright Epic Games, Inc. All Rights Reserved.

#include "TD_HandIKEditMode.h"
#include "TD_AnimGraphNodeHandIK.h"
#include "IPersonaPreviewScene.h"
#include "Animation/DebugSkelMeshComponent.h"


void FTD_HandIKEditMode::EnterMode(class UAnimGraphNode_Base* InEditorNode, struct FAnimNode_Base* InRuntimeNode)
{
	RuntimeNode = static_cast<FTD_AnimNodeHandIK*>(InRuntimeNode);
	GraphNode = CastChecked<UTD_AnimGraphNodeHandIK>(InEditorNode);
	UE_LOG(LogTemp, Warning, TEXT("EnterMode"));

	FTD_AnimModeBase::EnterMode(InEditorNode, InRuntimeNode);
}

void FTD_HandIKEditMode::ExitMode()
{
	UE_LOG(LogTemp, Warning, TEXT("ExitMode"));
	RuntimeNode = nullptr;
	GraphNode = nullptr;

	FTD_AnimModeBase::ExitMode();
}

FVector FTD_HandIKEditMode::GetWidgetLocation() const
{
	EBoneControlSpace Space = RuntimeNode->EffectorLocationSpace;
	FVector Location = RuntimeNode->EffectorLocation;
	FBoneSocketTarget Target = RuntimeNode->EffectorTarget;

	USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();
	UE_LOG(LogTemp, Warning, TEXT("GetWidgetLocation"));
	return Location;
	//return ConvertWidgetLocation(SkelComp, RuntimeNode->ForwardedPose, Target, Location, Space);
}

FWidget::EWidgetMode FTD_HandIKEditMode::GetWidgetMode() const
{
	USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();
	int32 TipBoneIndex = SkelComp->GetBoneIndex(RuntimeNode->TipBone.BoneName);
	int32 RootBoneIndex = SkelComp->GetBoneIndex(RuntimeNode->RootBone.BoneName);

	if (TipBoneIndex != INDEX_NONE && RootBoneIndex != INDEX_NONE)
	{
		return FWidget::WM_Translate;
	}
	UE_LOG(LogTemp, Warning, TEXT("GetWidgetMode"));
	return FWidget::WM_None;
}

void FTD_HandIKEditMode::DoTranslation(FVector& InTranslation)
{
	USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();

	FVector Offset = ConvertCSVectorToBoneSpace(SkelComp, InTranslation, RuntimeNode->ForwardedPose, RuntimeNode->EffectorTarget, GraphNode->Node.EffectorLocationSpace);

	RuntimeNode->EffectorLocation += Offset;
	GraphNode->Node.EffectorLocation = RuntimeNode->EffectorLocation;
	GraphNode->SetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FTD_AnimNodeHandIK, EffectorLocation), RuntimeNode->EffectorLocation);
	UE_LOG(LogTemp, Warning, TEXT("DoTranslation"));
}

void FTD_HandIKEditMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	UE_LOG(LogTemp, Warning, TEXT("Render"));
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// draw line from root bone to tip bone if available
	//if (RuntimeNode && RuntimeNode->DebugLines.Num() > 0)
	//{
	//	USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();
	//	FTransform CompToWorld = SkelComp->GetComponentToWorld();

	//	// no component space
	//	for (int32 Index = 1; Index < RuntimeNode->DebugLines.Num(); ++Index)
	//	{
	//		FVector Start = CompToWorld.TransformPosition(RuntimeNode->DebugLines[Index - 1]);
	//		FVector End = CompToWorld.TransformPosition(RuntimeNode->DebugLines[Index]);

	//		PDI->DrawLine(Start, End, FLinearColor::Red, SDPG_Foreground);
	//	}
	//}
#endif // #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}