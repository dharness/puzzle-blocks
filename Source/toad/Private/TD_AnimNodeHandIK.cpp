
#include "TD_AnimNodeHandIK.h"
#include "AnimationRuntime.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstanceProxy.h"


FTD_AnimNodeHandIK::FTD_AnimNodeHandIK()
	: EffectorLocation(FVector::ZeroVector)
#if WITH_EDITORONLY_DATA
	, bEnableDebugDraw(false)
#endif
{
	EffectorLocationSpace = EBoneControlSpace::BCS_WorldSpace;
}

FVector FTD_AnimNodeHandIK::GetCurrentLocation(FCSPose<FCompactPose>& MeshBases, const FCompactPoseBoneIndex& BoneIndex)
{
	return MeshBases.GetComponentSpaceTransform(BoneIndex).GetLocation();
}

FTransform FTD_AnimNodeHandIK::GetTargetTransform(const FTransform& InComponentTransform, FCSPose<FCompactPose>& MeshBases, FBoneSocketTarget& InTarget, EBoneControlSpace Space, const FTransform& InOffset)
{
	FTransform OutTransform;
	if (Space == BCS_BoneSpace)
	{
		OutTransform = InTarget.GetTargetTransform(InOffset, MeshBases, InComponentTransform);
	}
	else
	{
		// parent bone space still goes through this way
		// if your target is socket, it will try find parents of joint that socket belongs to
		OutTransform = InOffset;
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(InComponentTransform, MeshBases, OutTransform, InTarget.GetCompactPoseBoneIndex(), Space);
	}

	return OutTransform;
}

void FTD_AnimNodeHandIK::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	UE_LOG(LogTemp, Warning, TEXT("------------------------- EvaluateSkeletalControl_AnyThread -------------------------"));

	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	// Gather all bone indices between root and tip.
	TArray<FCompactPoseBoneIndex> BoneIndices;

	{
		const FCompactPoseBoneIndex RootIndex = RootBone.GetCompactPoseIndex(BoneContainer);
		FCompactPoseBoneIndex BoneIndex = TipBone.GetCompactPoseIndex(BoneContainer);
		do
		{
			BoneIndices.Insert(BoneIndex, 0);
			BoneIndex = Output.Pose.GetPose().GetParentBoneIndex(BoneIndex);
		} while (BoneIndex != RootIndex);
		BoneIndices.Insert(BoneIndex, 0);
	}

	// Maximum length of skeleton segment at full extension
	float MaximumReach = 0;
	TArray<FVector> BoneLocations;

	// Gather transforms
	int32 const NumTransforms = BoneIndices.Num();
	OutBoneTransforms.AddUninitialized(NumTransforms);
	BoneLocations.AddUninitialized(NumTransforms+1);

	// Start with Root Bone
	{
		const FCompactPoseBoneIndex& RootBoneIndex = BoneIndices[0];
		const FTransform& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(RootBoneIndex);
		auto BT = FBoneTransform(RootBoneIndex, BoneCSTransform);

		OutBoneTransforms[0] = BT;
		BoneLocations[0] = BT.Transform.GetLocation();
	}

	const FVector NormEffectorLocation = EffectorLocation.GetSafeNormal();

	// Go through remaining transforms

	for (int32 TransformIndex = 1; TransformIndex < NumTransforms; TransformIndex++)
	{
		const FCompactPoseBoneIndex& BoneIndex = BoneIndices[TransformIndex];

		const FTransform& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(BoneIndex);
		FVector const BoneCSLocation = BoneCSTransform.GetLocation();
		BoneLocations[TransformIndex] = BoneCSLocation;

		OutBoneTransforms[TransformIndex] = FBoneTransform(BoneIndex, BoneCSTransform);
	}

	for (int32 TransformIndex = 1; TransformIndex < NumTransforms; TransformIndex++)
	{
		const FCompactPoseBoneIndex& BoneIndex = BoneIndices[TransformIndex];
		FVector const BoneCSLocation = BoneLocations[TransformIndex];
		FVector const ParentBoneCSLocation = BoneLocations[TransformIndex - 1];

		// Calculate the combined length of this segment of skeleton
		float const BoneLength = FVector::Dist(BoneCSLocation, ParentBoneCSLocation);
		UE_LOG(LogTemp, Warning, TEXT("BoneLength: %f"), BoneLength);

		if (!FMath::IsNearlyZero(BoneLength))
		{
			MaximumReach += BoneLength;
		}
		OutBoneTransforms[TransformIndex].Transform.SetLocation(NormEffectorLocation* MaximumReach);
	}

	
	return;
}

bool FTD_AnimNodeHandIK::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	// Allow evaluation if all parameters are initialized and TipBone is child of RootBone
	return
		(
			TipBone.IsValidToEvaluate(RequiredBones)
			&& RootBone.IsValidToEvaluate(RequiredBones)
			&& RequiredBones.BoneIsChildOf(TipBone.BoneIndex, RootBone.BoneIndex)
			);
}

void FTD_AnimNodeHandIK::ConditionalDebugDraw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* PreviewSkelMeshComp) const
{
#if WITH_EDITORONLY_DATA

	if (bEnableDebugDraw && PreviewSkelMeshComp && PreviewSkelMeshComp->GetWorld())
	{
		FVector const CSEffectorLocation = CachedEffectorCSTransform.GetLocation();

		// Show end effector position.
		//DrawDebugBox(PreviewSkelMeshComp->GetWorld(), CSEffectorLocation, FVector(Precision), FColor::Green, true, 0.1f);
		DrawDebugCoordinateSystem(PreviewSkelMeshComp->GetWorld(), CSEffectorLocation, CachedEffectorCSTransform.GetRotation().Rotator(), 5.f, true, 0.1f);
	}
#endif
}

void FTD_AnimNodeHandIK::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)
	TipBone.Initialize(RequiredBones);
	RootBone.Initialize(RequiredBones);
}

void FTD_AnimNodeHandIK::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	FString DebugLine = DebugData.GetNodeName(this);

	DebugData.AddDebugItem(DebugLine);
	ComponentPose.GatherDebugData(DebugData);
}

void FTD_AnimNodeHandIK::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	Super::Initialize_AnyThread(Context);
}
