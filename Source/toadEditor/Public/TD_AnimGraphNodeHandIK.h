#pragma once

#include "AnimGraphNode_Base.h"
#include "TD_AnimNodeHandIK.h"
#include "TD_AnimGraphNodeHandIK.generated.h"

UCLASS()
class TOADEDITOR_API UTD_AnimGraphNodeHandIK : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Settings)
	FTD_AnimNodeHandIK Node;

	//~ Begin UEdGraphNode Interface.
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UAnimGraphNode_Base Interface
	virtual FString GetNodeCategory() const override;
	//~ End UAnimGraphNode_Base Interface

	UTD_AnimGraphNodeHandIK(const FObjectInitializer& ObjectInitializer);

protected:
	virtual FText GetControllerDescription() const;
};