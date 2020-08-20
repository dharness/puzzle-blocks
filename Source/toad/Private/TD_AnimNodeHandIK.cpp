
#include "TD_AnimNodeHandIK.h"
#include "AnimationRuntime.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstanceProxy.h"
#include "TD_HandIKCore.h"


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
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	FVector const CSEffectorLocation = EffectorLocation;

	// Gather all bone indices between root and tip.
	TArray<FCompactPoseBoneIndex> BoneIndices;

	const int32 BoneCount = CachedBoneReferences.Num();
	for (int32 BoneIndex = 0; BoneIndex < BoneCount; BoneIndex++)
	{
		const FTD_HandIKCachedBoneData& BoneData = CachedBoneReferences[BoneIndex];
		if (!BoneData.Bone.IsValidToEvaluate(BoneContainer))
		{
			break;
		}
		BoneIndices.Add(BoneData.Bone.GetCompactPoseIndex(BoneContainer));
	}

	//{
	//	const FCompactPoseBoneIndex RootIndex = RootBone.GetCompactPoseIndex(BoneContainer);
	//	FCompactPoseBoneIndex BoneIndex = TipBone.GetCompactPoseIndex(BoneContainer);
	//	do
	//	{
	//		BoneIndices.Insert(BoneIndex, 0);
	//		BoneIndex = Output.Pose.GetPose().GetParentBoneIndex(BoneIndex);
	//	} while (BoneIndex != RootIndex);
	//	BoneIndices.Insert(BoneIndex, 0);
	//}

	// Maximum length of skeleton segment at full extension
	float MaximumReach = 0;

	// Gather transforms
	int32 const NumTransforms = BoneIndices.Num();
	OutBoneTransforms.AddUninitialized(NumTransforms);

	// Gather chain links. These are non zero length bones.
	TArray<FHandIKChainLink> CurrentChain;
	CurrentChain.Reserve(NumTransforms);

	// Start with Root Bone
	{
		const FCompactPoseBoneIndex& RootBoneIndex = BoneIndices[0];
		const FTransform& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(RootBoneIndex);

		OutBoneTransforms[0] = FBoneTransform(RootBoneIndex, BoneCSTransform);
		CurrentChain.Add(FHandIKChainLink(BoneCSTransform.GetLocation(), 0.f, RootBoneIndex, 0));
	}

	// Go through remaining transforms
	for (int32 TransformIndex = 1; TransformIndex < NumTransforms; TransformIndex++)
	{
		const FCompactPoseBoneIndex& BoneIndex = BoneIndices[TransformIndex];

		const FTransform& BoneCSTransform = Output.Pose.GetComponentSpaceTransform(BoneIndex);
		FVector const BoneCSPosition = BoneCSTransform.GetLocation();

		OutBoneTransforms[TransformIndex] = FBoneTransform(BoneIndex, BoneCSTransform);

		// Calculate the combined length of this segment of skeleton
		float const BoneLength = CachedBoneLengths[TransformIndex];

		if (!FMath::IsNearlyZero(BoneLength))
		{
			CurrentChain.Add(FHandIKChainLink(BoneCSPosition, BoneLength, BoneIndex, TransformIndex));
			MaximumReach += BoneLength;
		}
		else
		{
			// Mark this transform as a zero length child of the last link.
			// It will inherit position and delta rotation from parent link.
			FHandIKChainLink& ParentLink = CurrentChain[CurrentChain.Num() - 1];
			ParentLink.ChildZeroLengthTransformIndices.Add(TransformIndex);
		}
	}

	int32 const NumChainLinks = CurrentChain.Num();

	const bool bBoneLocationUpdated = TD_AnimationCore::SolveHandIK(CurrentChain, CSEffectorLocation, ControlPointWeight,
	                                                                MaximumReach, HandIKDebugData);

	// If we moved some bones, update bone transforms.
	if (bBoneLocationUpdated)
	{
		// First step: update bone transform positions from chain links.
		for (int32 LinkIndex = 0; LinkIndex < NumChainLinks; LinkIndex++)
		{
			FHandIKChainLink const& ChainLink = CurrentChain[LinkIndex];
			OutBoneTransforms[ChainLink.TransformIndex].Transform.SetTranslation(ChainLink.Position);

			// If there are any zero length children, update position of those
			int32 const NumChildren = ChainLink.ChildZeroLengthTransformIndices.Num();
			for (int32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
			{
				OutBoneTransforms[ChainLink.ChildZeroLengthTransformIndices[ChildIndex]].Transform.SetTranslation(ChainLink.Position);
			}
		}

		for (int32 LinkIndex = 0; LinkIndex < NumChainLinks - 1; LinkIndex++)
		{
			FHandIKChainLink const& CurrentLink = CurrentChain[LinkIndex];
			FHandIKChainLink const& ChildLink = CurrentChain[LinkIndex + 1];

			// Calculate pre-translation vector between this bone and child
			FVector const OldDir = (GetCurrentLocation(Output.Pose, FCompactPoseBoneIndex(ChildLink.BoneIndex)) - GetCurrentLocation(Output.Pose, FCompactPoseBoneIndex(CurrentLink.BoneIndex))).GetUnsafeNormal();

			// Get vector from the post-translation bone to it's child
			FVector const NewDir = (ChildLink.Position - CurrentLink.Position).GetUnsafeNormal();

			// Calculate axis of rotation from pre-translation vector to post-translation vector
			FVector const RotationAxis = FVector::CrossProduct(OldDir, NewDir).GetSafeNormal();
			float const RotationAngle = FMath::Acos(FVector::DotProduct(OldDir, NewDir));
			FQuat const DeltaRotation = FQuat(RotationAxis, RotationAngle);
			// We're going to multiply it, in order to not have to re-normalize the final quaternion, it has to be a unit quaternion.
			checkSlow(DeltaRotation.IsNormalized());

			// Calculate absolute rotation and set it
			FTransform& CurrentBoneTransform = OutBoneTransforms[CurrentLink.TransformIndex].Transform;
			CurrentBoneTransform.SetRotation(DeltaRotation * CurrentBoneTransform.GetRotation());
			CurrentBoneTransform.NormalizeRotation();

			// Update zero length children if any
			int32 const NumChildren = CurrentLink.ChildZeroLengthTransformIndices.Num();
			for (int32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
			{
				FTransform& ChildBoneTransform = OutBoneTransforms[CurrentLink.ChildZeroLengthTransformIndices[ChildIndex]].Transform;
				ChildBoneTransform.SetRotation(DeltaRotation * ChildBoneTransform.GetRotation());
				ChildBoneTransform.NormalizeRotation();
			}
		}
#if WITH_EDITOR
		Chain = CurrentChain;
#endif // WITH_EDITOR

	}

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

	//if (bEnableDebugDraw && PreviewSkelMeshComp && PreviewSkelMeshComp->GetWorld())
	//{
	//	FVector const CSEffectorLocation = CachedEffectorCSTransform.GetLocation();

	//	// Show end effector position.
	//	//DrawDebugBox(PreviewSkelMeshComp->GetWorld(), CSEffectorLocation, FVector(Precision), FColor::Green, true, 0.1f);
	//	DrawDebugCoordinateSystem(PreviewSkelMeshComp->GetWorld(), CSEffectorLocation, CachedEffectorCSTransform.GetRotation().Rotator(), 5.f, true, 0.1f);
	//}
#endif
}

void FTD_AnimNodeHandIK::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)
	TipBone.Initialize(RequiredBones);
	RootBone.Initialize(RequiredBones);

	GatherBoneReferences(RequiredBones.GetReferenceSkeleton());

	for (FTD_HandIKCachedBoneData& CachedBoneData : CachedBoneReferences)
	{
		CachedBoneData.Bone.Initialize(RequiredBones);
	}
}

void FTD_AnimNodeHandIK::GatherBoneReferences(const FReferenceSkeleton& RefSkeleton)
{
	CachedBoneReferences.Reset();

	int32 RootIndex = RefSkeleton.FindBoneIndex(RootBone.BoneName);
	int32 TipIndex = RefSkeleton.FindBoneIndex(TipBone.BoneName);

	// Loop backwards from tip to root inserting on the lefts side of the array
	int32 BoneIndex = TipIndex;
	do
	{
		FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
		CachedBoneReferences.EmplaceAt(0, BoneName, BoneIndex);
		BoneIndex = RefSkeleton.GetParentIndex(BoneIndex);
	} while (BoneIndex != RootIndex);

	FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
	CachedBoneReferences.EmplaceAt(0, BoneName, BoneIndex);

	GatherBoneLengths(RefSkeleton);
}

void FTD_AnimNodeHandIK::GatherBoneLengths(const FReferenceSkeleton& RefSkeleton)
{
	if (CachedBoneReferences.Num() > 0)
	{
		TArray<FTransform> ComponentSpaceTransforms;
		FAnimationRuntime::FillUpComponentSpaceTransforms(RefSkeleton, RefSkeleton.GetRefBonePose(), ComponentSpaceTransforms);

		// Build cached bone info
		CachedBoneLengths.Reset();
		for (int32 BoneIndex = 0; BoneIndex < CachedBoneReferences.Num(); BoneIndex++)
		{
			float BoneLength = 0.0f;

			if (BoneIndex > 0)
			{
				FTD_HandIKCachedBoneData& BoneData = CachedBoneReferences[BoneIndex];
				const FTransform& Transform = ComponentSpaceTransforms[BoneData.RefSkeletonIndex];

				const FTD_HandIKCachedBoneData& ParentBoneData = CachedBoneReferences[BoneIndex - 1];
				const FTransform& ParentTransform = ComponentSpaceTransforms[ParentBoneData.RefSkeletonIndex];
				const FVector BoneDir = Transform.GetLocation() - ParentTransform.GetLocation();
				BoneLength = BoneDir.Size();
			}
			CachedBoneLengths.Add(BoneLength);
		}
	}
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

//-----------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------- FTD_BezierCurveCache ---------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------


void FTD_BezierCurveCache::Add(float ArcLength, FVector CurvePosition)
{
	CurveCache.Add(MakeTuple(ArcLength, CurvePosition));
}

void FTD_BezierCurveCache::Empty()
{
	CurveCache.Empty();
}

FVector FTD_BezierCurveCache::FindNearest(float ArcLength)
{
	FVector NearestCurveValue;

	//UE_LOG(LogTemp, Warning, TEXT("ArcLength: %f, T: %f"), ArcLength, T);
	for (int i = 0; i < CurveCache.Num() - 1; i++)
	{
		const auto CacheItem = CurveCache[i];
		const auto NextCacheItem = CurveCache[i + 1];
		const float CacheArcLength = CacheItem.Key;
		const float NextCacheArcLength = NextCacheItem.Key;
		NearestCurveValue = CacheItem.Value;

		if (ArcLength >= CacheArcLength && ArcLength < NextCacheArcLength)
		{
			break;
		}
	}
	return NearestCurveValue;
}
