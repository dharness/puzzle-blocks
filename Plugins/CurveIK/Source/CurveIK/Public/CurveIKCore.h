#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BoneIndices.h"
#include "CurveIKCore.generated.h"


USTRUCT()
struct FCurveIK_CurveCache
{
	GENERATED_BODY()

public:
	void Add(float ArcLength, FVector CurvePosition);

	void Empty();

	FVector FindNearest(float ArcLength);

private:
	TArray<TTuple<float, FVector>> CurveCache;
};


USTRUCT()
struct FCurveIKChainLink
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

	FCurveIKChainLink()
		: Position(FVector::ZeroVector)
		, Length(0.f)
		, BoneIndex(INDEX_NONE)
		, TransformIndex(INDEX_NONE)
		, DefaultDirToParent(FVector(-1.f, 0.f, 0.f))
	{
	}

	FCurveIKChainLink(const FVector& InPosition, const float InLength, const FCompactPoseBoneIndex& InBoneIndex, const int32& InTransformIndex)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex.GetInt())
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(FVector(-1.f, 0.f, 0.f))
	{
	}

	FCurveIKChainLink(const FVector& InPosition, const float InLength, const FCompactPoseBoneIndex& InBoneIndex, const int32& InTransformIndex, const FVector& InDefaultDirToParent)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex.GetInt())
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(InDefaultDirToParent)
	{
	}

	FCurveIKChainLink(const FVector& InPosition, const float InLength, const int32 InBoneIndex, const int32 InTransformIndex)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex)
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(FVector(-1.f, 0.f, 0.f))
	{
	}
};

USTRUCT()
struct FCurveIKDebugData
{
	GENERATED_BODY()

public:
	FVector ControlPoint;
	FVector ControlVector;
	FVector P1;
	FVector P2;
	FVector RightVector;
	FVector UpVector;

};

namespace CurveIK_AnimationCore
{
	CURVEIK_API bool SolveCurveIK(TArray<FCurveIKChainLink>& InOutChain, const FVector& TargetLocation,
		float ControlPointWeight, float MaximumReach, FCurveIKDebugData& CurveIKDebugData);
	
};
