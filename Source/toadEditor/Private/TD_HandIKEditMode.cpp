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
	return Location;
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
	return FWidget::WM_None;
}

void FTD_HandIKEditMode::DoTranslation(FVector& InTranslation)
{
	USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();

	FVector Offset = ConvertCSVectorToBoneSpace(SkelComp, InTranslation, RuntimeNode->ForwardedPose, RuntimeNode->EffectorTarget, GraphNode->Node.EffectorLocationSpace);

	RuntimeNode->EffectorLocation += Offset;
	GraphNode->Node.EffectorLocation = RuntimeNode->EffectorLocation;
	GraphNode->SetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FTD_AnimNodeHandIK, EffectorLocation), RuntimeNode->EffectorLocation);
}

void FTD_HandIKEditMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// draw line from root bone to tip bone if available
	if (RuntimeNode && RuntimeNode->bEnableDebugDraw)
	{
		USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();
		FTransform CompToWorld = SkelComp->GetComponentToWorld();
		FHandIKDebugData HandIKDebugData = RuntimeNode->HandIKDebugData;
		const FVector MidPoint = HandIKDebugData.P1 + (HandIKDebugData.P2 / 2.0);
		const FVector P1 = HandIKDebugData.P1;
		const FVector P2 = HandIKDebugData.P2;
		const FVector ControlPoint = HandIKDebugData.ControlPoint;

		const float LineScale = 30;
		PDI->DrawPoint(P1, FLinearColor::Blue, 15, SDPG_Foreground);
		PDI->DrawPoint(P2, FLinearColor::Blue, 15, SDPG_Foreground);
		PDI->DrawPoint(ControlPoint, FLinearColor::Red, 15, SDPG_Foreground);
		

		// P Vector
		PDI->DrawLine(
			P1,
			P2,
			FLinearColor::FromSRGBColor(FColor::Cyan),
			SDPG_Foreground
		);
		
		PDI->DrawLine(
			MidPoint,
			MidPoint + (HandIKDebugData.ControlVector * LineScale),
			FLinearColor::FromSRGBColor(FColor::Red),
			SDPG_Foreground
		);

		const auto MidPointOriginal= (P1 + (HandIKDebugData.UpVector * LineScale)) / 2;
		PDI->DrawLine(
			MidPointOriginal,
			MidPointOriginal + (HandIKDebugData.RightVector.GetSafeNormal() * LineScale),
			FLinearColor::FromSRGBColor(FColor::Orange),
			SDPG_Foreground
		);
	
		PDI->DrawLine(
			P1,
			P1 + (HandIKDebugData.UpVector * LineScale),
			FLinearColor::FromSRGBColor(FColor::Orange),
			SDPG_Foreground
		);
		for(int i = 1; i < RuntimeNode->Chain.Num(); i++)
		{
			FHandIKChainLink ChainLink = RuntimeNode->Chain[i];
			PDI->DrawPoint(ChainLink.Position, FLinearColor::FromSRGBColor(FColor::Orange), 30, SDPG_Foreground);
			FHandIKChainLink ParentChainLink = RuntimeNode->Chain[i - 1];
			//PDI->DrawLine(
			//	ChainLink.Position,
			//	ParentChainLink.Position,
			//	FLinearColor::FromSRGBColor(FColor::Blue),
			//	SDPG_Foreground
			//);
		}
	}
#endif // #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}