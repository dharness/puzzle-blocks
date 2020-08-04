#pragma once

#include "CoreMinimal.h"
#include "TD_IKArgs.generated.h"

USTRUCT()
struct FTD_IKArgs
{
    GENERATED_BODY()

    UPROPERTY()
    float TimelineValue = 0;

    UPROPERTY()
    USceneComponent* HandIKTargetR;
};