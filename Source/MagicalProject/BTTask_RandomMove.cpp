// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RandomMove.h"
#include "BehaviorTree/BTNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "NavigationSystem.h"
#include "MonsterAI.h"
#include "Engine.h"
#include "GameFramework/Pawn.h"
#include "UObject/UObjectGlobals.h"
#include "BlackBoardKeys.h"

UBTTask_RandomMove::UBTTask_RandomMove(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Random Location");
	SearchRadius = 1500.f;
}

EBTNodeResult::Type UBTTask_RandomMove::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	auto const cont = Cast<AMonsterAI>(owner_comp.GetAIOwner());
	auto const npc = cont->GetPawn();

	if (npc)
	{
		FVector const origin = npc->GetActorLocation();
		FNavLocation loc;

		UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nav_sys->GetRandomPointInNavigableRadius(origin, SearchRadius, loc, nullptr))
			cont->GetBlackBoard()->SetValueAsVector(bb_keys::target_location, loc.Location);
	}


	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
