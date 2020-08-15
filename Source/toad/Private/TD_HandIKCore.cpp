#include "TD_HandIKCore.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

namespace TD_AnimationCore
{
	FVector QuadraticBezier(const FVector P1, const FVector Control, const FVector P2, const float T)
	{
		FVector PFinal;
		PFinal.X = FGenericPlatformMath::Pow(1 - T, 2) * P1.X +
			(1 - T) * 2 * T * Control.X +
			T * T * P2.X;
		PFinal.Y = FGenericPlatformMath::Pow(1 - T, 2) * P1.Y +
			(1 - T) * 2 * T * Control.Y +
			T * T * P2.Y;
		PFinal.Z = FGenericPlatformMath::Pow(1 - T, 2) * P1.Z +
			(1 - T) * 2 * T * Control.Z +
			T * T * P2.Z;
		return PFinal;
	}

	FVector GetDefaultControlDirection(const FVector P1, const FVector P2, const FVector ComponentRightVector, FHandIKDebugData& HandIKDebugData)
	{
		const FVector P = (P2 - P1);
		const FVector DirToNextPoint = P.GetSafeNormal();
		const FVector RightVector = ComponentRightVector;
		const FVector Projection = DirToNextPoint * FVector::DotProduct(RightVector, DirToNextPoint) / FMath::Square(DirToNextPoint.Size());
		const FVector ProjPR = (FVector::DotProduct(P, RightVector) * RightVector) / FMath::Square(RightVector.Size());
		FVector UnrealControl = FVector::VectorPlaneProject(RightVector, DirToNextPoint);

		FVector ControlVector = Projection - RightVector;
		//if (DirToNextPoint.Z < 0)
		//{
		//	ControlVector = ControlVector * FVector(1,1,-1);
		//	UnrealControl = UnrealControl * -1;
		//}
		//ControlVector.Normalize();
		HandIKDebugData.POriginal = FVector::UpVector;
		HandIKDebugData.ControlOriginal = FVector::LeftVector;
		HandIKDebugData.P1 = P1;
		HandIKDebugData.P2 = P2;
		HandIKDebugData.Projection = Projection;
		HandIKDebugData.ControlVector = ControlVector;
		HandIKDebugData.ProjPR = ProjPR;
		HandIKDebugData.UnrealControl = UnrealControl;
		
		return ControlVector;
	}

	bool SolveHandIK(TArray<FHandIKChainLink>& InOutChain, const FVector& TargetPosition, float MaximumReach, FHandIKDebugData& HandIKDebugData)
	{
		bool bBoneLocationUpdated = false;
		float const RootToTargetDistSq = FVector::DistSquared(InOutChain[0].Position, TargetPosition);
		int32 const NumChainLinks = InOutChain.Num();
		FVector RightVector = FVector::RightVector;

		// If the effector is further away than the distance from root to tip, simply move all bones in a line from root to effector location
		if (RootToTargetDistSq > FMath::Square(MaximumReach))
		{
			for (int32 LinkIndex = 1; LinkIndex < NumChainLinks; LinkIndex++)
			{
				FHandIKChainLink const& ParentLink = InOutChain[LinkIndex - 1];
				FHandIKChainLink& CurrentLink = InOutChain[LinkIndex];
				CurrentLink.Position = ParentLink.Position + (TargetPosition - ParentLink.Position).GetUnsafeNormal() * CurrentLink.Length;
			}
			bBoneLocationUpdated = true;
		}
		else // Effector is within reach, calculate bone translations to position tip at effector location
		{
			FVector P1 = InOutChain[0].Position;
			P1.X = 0;
			const FVector P2 = TargetPosition;
			const FVector MidPoint = P1 + (P2/2.0);
			FVector ControlVector = GetDefaultControlDirection(P1, P2, RightVector, HandIKDebugData);
			const FVector ControlPoint = MidPoint + (ControlVector * 15);
			HandIKDebugData.ControlPoint = ControlPoint;
			HandIKDebugData.RightVector = RightVector;

			//const FVector PFinal = QuadraticBezier

			float TotalLength = 0;
			for(int LinkIndex = 0; LinkIndex < NumChainLinks; LinkIndex++)
			{
				FHandIKChainLink& CurrentLink = InOutChain[LinkIndex];
				TotalLength += CurrentLink.Length;
				const float T = TotalLength / MaximumReach;
				const FVector BonePosition = QuadraticBezier(P1, ControlPoint, P2, T);
				CurrentLink.Position = BonePosition;
			}
			bBoneLocationUpdated = true;
		}
		return bBoneLocationUpdated;
	}
};
