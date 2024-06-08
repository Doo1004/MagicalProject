// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class MAGICALPROJECT_API UBTTask_FindPlayerLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	UBTTask_FindPlayerLocation(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* nodememory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	float SearchRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	bool IsRandomSearch;
};
