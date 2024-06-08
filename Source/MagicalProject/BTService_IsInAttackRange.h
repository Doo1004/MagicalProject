// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class MAGICALPROJECT_API UBTService_IsInAttackRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_IsInAttackRange();
	void OnBecomeRelevant(UBehaviorTreeComponent& owner_comp, uint8* node_memory) override;
};
