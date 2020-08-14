#include "TD_HandIKCore.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

namespace TD_AnimationCore
{
	FVector QuadraticBezier(const FVector P0, const FVector P1, const FVector P2, const float T)
	{
		FVector PFinal;
		PFinal.X = FGenericPlatformMath::Pow(1 - T, 2) * P0.X +
			(1 - T) * 2 * T * P1.X +
			T * T * P2.X;
		PFinal.Y = FGenericPlatformMath::Pow(1 - T, 2) * P0.Y +
			(1 - T) * 2 * T * P1.Y +
			T * T * P2.Y;
		PFinal.Z = FGenericPlatformMath::Pow(1 - T, 2) * P0.Z +
			(1 - T) * 2 * T * P1.Z +
			T * T * P2.Z;
		return PFinal;
	}

	bool SolveHandIK(TArray<FHandIKChainLink>& InOutChain, const FVector& TargetPosition, float MaximumReach, TArray<FVector>& Points)
	{
		bool bBoneLocationUpdated = false;
		float const RootToTargetDistSq = FVector::DistSquared(InOutChain[0].Position, TargetPosition);
		int32 const NumChainLinks = InOutChain.Num();

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
			const FVector P0 = InOutChain[0].Position;
			const FVector P2 = TargetPosition;
			const FVector P1 = P0 + (P2/2.0);
			Points.Add(P0);
			Points.Add(P1);
			Points.Add(P2);

			//const FVector PFinal = QuadraticBezier

			float TotalLength = 0;
			for(int LinkIndex = 0; LinkIndex < NumChainLinks; LinkIndex++)
			{
				FHandIKChainLink& CurrentLink = InOutChain[LinkIndex];
				TotalLength += CurrentLink.Length;
				const float T = TotalLength / MaximumReach;
				const FVector BonePosition = QuadraticBezier(P0, P1, P2, T);
				CurrentLink.Position = BonePosition;
			}
			bBoneLocationUpdated = true;
		}
		return bBoneLocationUpdated;
	}
};
