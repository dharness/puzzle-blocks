#include "TD_AnimNodeHandIK.h"
#include "Animation/AnimInstanceProxy.h"


FTD_AnimNodeHandIK::FTD_AnimNodeHandIK()
{

}

void FTD_AnimNodeHandIK::Initialize(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);

	BasePose.Initialize(Context);
}

void FTD_AnimNodeHandIK::CacheBones(const FAnimationCacheBonesContext& Context)
{
	BasePose.CacheBones(Context);
}

void FTD_AnimNodeHandIK::Update(const FAnimationUpdateContext& Context)
{
	//EvaluateGraphExposedInputs.Execute(Context);
	BasePose.Update(Context);
}

void FTD_AnimNodeHandIK::Evaluate(FPoseContext& Output)
{
	// Evaluate the input
	BasePose.Evaluate(Output);
}

void FTD_AnimNodeHandIK::GatherDebugData(FNodeDebugData& DebugData)
{
	FString DebugLine = DebugData.GetNodeName(this);


	DebugData.AddDebugItem(DebugLine);

	BasePose.GatherDebugData(DebugData);
}