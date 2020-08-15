#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BoneIndices.h"
#include "TD_HandIKCore.generated.h"


USTRUCT()
struct FHandIKChainLink
{
	GENERATED_BODY()

public:
	/** Position of bone in component space. */
	FVector Position;

	/** Distance to its parent link. */
	float Length;

	/** Bone Index in SkeletalMesh */
	int32 BoneIndex;

	/** Transform Index that this control will output */
	int32 TransformIndex;

	/** Default Direction to Parent */
	FVector DefaultDirToParent;

	/** Child bones which are overlapping this bone.
	* They have a zero length distance, so they will inherit this bone's transformation. */
	TArray<int32> ChildZeroLengthTransformIndices;

	FHandIKChainLink()
		: Position(FVector::ZeroVector)
		, Length(0.f)
		, BoneIndex(INDEX_NONE)
		, TransformIndex(INDEX_NONE)
		, DefaultDirToParent(FVector(-1.f, 0.f, 0.f))
	{
	}

	FHandIKChainLink(const FVector& InPosition, const float InLength, const FCompactPoseBoneIndex& InBoneIndex, const int32& InTransformIndex)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex.GetInt())
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(FVector(-1.f, 0.f, 0.f))
	{
	}

	FHandIKChainLink(const FVector& InPosition, const float InLength, const FCompactPoseBoneIndex& InBoneIndex, const int32& InTransformIndex, const FVector& InDefaultDirToParent)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex.GetInt())
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(InDefaultDirToParent)
	{
	}

	FHandIKChainLink(const FVector& InPosition, const float InLength, const int32 InBoneIndex, const int32 InTransformIndex)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex)
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(FVector(-1.f, 0.f, 0.f))
	{
	}
};

USTRUCT()
struct FHandIKDebugData
{
	GENERATED_BODY()

public:
	FVector ControlPoint;
	FVector POriginal;
	FVector ControlOriginal;
	FVector P1;
	FVector P2;
	FVector RightVector;
	FVector Projection;
	FVector ControlVector;
	FVector ProjPR;
	FVector UnrealControl;

};

namespace TD_AnimationCore
{
	TOAD_API bool SolveHandIK(TArray<FHandIKChainLink>& InOutChain, const FVector& TargetLocation, float MaximumReach, FHandIKDebugData& HandIKDebugData);
};
