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
	if (RuntimeNode && RuntimeNode->bEnableDebugDraw && RuntimeNode->DebugLines.Num() > 0)
	{
		USkeletalMeshComponent* SkelComp = GetAnimPreviewScene().GetPreviewMeshComponent();
		FTransform CompToWorld = SkelComp->GetComponentToWorld();

		// no component space
		UE_LOG(LogTemp, Warning, TEXT("DebugLines.Num(): %d"), RuntimeNode->DebugLines.Num());
		for (int32 Index = 0; Index < RuntimeNode->DebugLines.Num(); ++Index)
		{
			FVector Point = CompToWorld.TransformPosition(RuntimeNode->DebugLines[Index]);
			if(Index % 2 != 0)
			{
				const FVector DirToNextPoint = (RuntimeNode->DebugLines[Index + 1] - Point).GetSafeNormal();
				FVector RightVector = FVector::RightVector;
				FVector Projection = DirToNextPoint*FVector::DotProduct(RightVector, DirToNextPoint)/FMath::Square(DirToNextPoint.Size());
				FVector PoleVector = Projection - RightVector;

				if (DirToNextPoint.Z < 0)
				{
					PoleVector *= -1;
				}

				RuntimeNode->Bead.SetLocation(Point);
				RuntimeNode->Bead.SetRotation(DirToNextPoint.Rotation().Quaternion());
				auto R = RightVector * 30;
				auto P2 = Projection * 30;
				auto V = PoleVector * 30;

				// R
				PDI->DrawLine(FVector::ZeroVector,  R, FLinearColor::FromSRGBColor(FColor::Cyan), SDPG_Foreground);
				// P2
				PDI->DrawLine(FVector::ZeroVector, P2, FLinearColor::FromSRGBColor(FColor::Red), SDPG_Foreground);
				// V
				PDI->DrawLine(R, R + V, FLinearColor::FromSRGBColor(FColor::Yellow), SDPG_Foreground);
				PDI->DrawLine(Point, Point + V, FLinearColor::FromSRGBColor(FColor::Yellow), SDPG_Foreground);
			}

			PDI->DrawPoint(Point, FLinearColor::Blue, 15, SDPG_Foreground);
		}
	}
#endif // #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}

//void FTD_HandIKEditMode::DrawCircle(FVector Center, float Radius, FPrimitiveDrawInterface* PDI)
//{
//	const int NumSubdivisions = 100;
//	
//}