#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BoneIndices.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "TD_HandIKCore.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "TD_AnimNodeHandIK.generated.h"

class FPrimitiveDrawInterface;
class USkeletalMeshComponent;


USTRUCT(BlueprintType)
struct TOAD_API FTD_AnimNodeHandIK : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Effector, meta = (PinShownByDefault))
	FVector EffectorLocation;

	UPROPERTY(EditAnywhere, Category = Effector)
	FBoneSocketTarget EffectorTarget;

	UPROPERTY(EditAnywhere, Category = Effector)
	TEnumAsByte<enum EBoneControlSpace> EffectorLocationSpace;

	/** Name of tip bone */
	UPROPERTY(EditAnywhere, Category = Solver)
    FBoneReference TipBone;

	/** Name of the root bone*/
	UPROPERTY(EditAnywhere, Category = Solver)
    FBoneReference RootBone;

#if WITH_EDITORONLY_DATA
	/** Toggle drawing of axes to debug joint rotation*/
	UPROPERTY(EditAnywhere, Category = Solver)
		bool bEnableDebugDraw;
#endif

public:
	FTD_AnimNodeHandIK();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	virtual void ConditionalDebugDraw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* PreviewSkelMeshComp) const;

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	// Convenience function to get current (pre-translation iteration) component space location of bone by bone index
	FVector GetCurrentLocation(FCSPose<FCompactPose>& MeshBases, const FCompactPoseBoneIndex& BoneIndex);
	static FTransform GetTargetTransform(const FTransform& InComponentTransform, FCSPose<FCompactPose>& MeshBases, FBoneSocketTarget& InTarget, EBoneControlSpace Space, const FTransform& InOffset);

	FHandIKDebugData TD_DebugData;

#if WITH_EDITORONLY_DATA
	// Cached CS location when in editor for debug drawing
	FTransform CachedEffectorCSTransform;
#endif
};
