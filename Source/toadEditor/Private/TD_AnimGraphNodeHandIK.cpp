#include "TD_AnimGraphNodeHandIK.h"
#include "Animation/AnimInstance.h"
#include "AnimNodeEditModes.h"
#include "AnimationCustomVersion.h"
#include "TD_AnimEditModes.h"
#include "toadEditor.h"


const FEditorModeID TD_AnimNodeEditModes::TDHandIK("UTD_AnimGraphNodeHandIK.TDHandIK");

UTD_AnimGraphNodeHandIK::UTD_AnimGraphNodeHandIK(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UTD_AnimGraphNodeHandIK::GetControllerDescription() const
{
	return FText::FromString(FString("TD Hand IK"));
}

void UTD_AnimGraphNodeHandIK::Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* PreviewSkelMeshComp) const
{
	if (PreviewSkelMeshComp)
	{
		if (FTD_AnimNodeHandIK* ActiveNode = GetActiveInstanceNode<FTD_AnimNodeHandIK>(PreviewSkelMeshComp->GetAnimInstance()))
		{
			ActiveNode->ConditionalDebugDraw(PDI, PreviewSkelMeshComp);
		}
	}
}

FText UTD_AnimGraphNodeHandIK::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

void UTD_AnimGraphNodeHandIK::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
	FTD_AnimNodeHandIK* TDHandIKNode = static_cast<FTD_AnimNodeHandIK*>(InPreviewNode);

	// copies Pin values from the internal node to get data which are not compiled yet
	TDHandIKNode->EffectorLocation = Node.EffectorLocation;
}

FEditorModeID UTD_AnimGraphNodeHandIK::GetEditorMode() const
{
	return TD_AnimNodeEditModes::TDHandIK;
}

void UTD_AnimGraphNodeHandIK::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FAnimationCustomVersion::GUID);
}
