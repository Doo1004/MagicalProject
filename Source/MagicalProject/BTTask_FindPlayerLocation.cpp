// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "MonsterAI.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "GameFramework/Character.h"
#include "BlackBoardKeys.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find Player Location");
	SearchRadius = 1500.f;
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* nodememory)
{
	ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	auto const cont = Cast<AMonsterAI>(owner_comp.GetAIOwner());

	FVector const vPlayerLocation = Player->GetActorLocation();
	if (IsRandomSearch)
	{
		FNavLocation loc;

		UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nav_sys->GetRandomPointInNavigableRadius(vPlayerLocation, SearchRadius, loc, nullptr))
			cont->GetBlackBoard()->SetValueAsVector(bb_keys::target_location, loc.Location);
	}
	else
		cont->GetBlackBoard()->SetValueAsVector(bb_keys::target_location, vPlayerLocation);

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
