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
			MidPointOriginal+ (HandIKDebugData.RightVector.GetSafeNormal() * LineScale),
			FLinearColor::FromSRGBColor(FColor::Orange),
			SDPG_Foreground
		);
	
		PDI->DrawLine(
			P1,
			P1 + (HandIKDebugData.UpVector * LineScale),
			FLinearColor::FromSRGBColor(FColor::Orange),
			SDPG_Foreground
		);
	}
#endif // #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}

//void FTD_HandIKEditMode::DrawCircle(FVector Center, float Radius, FVector ZeroVector, FPrimitiveDrawInterface* PDI)
//{
//	const int NumSubdivisions = 100;
//	float Deg = 0;
//	while(Deg < 360)
//	{
//		
//	}
//	
//}