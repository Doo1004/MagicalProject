// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsInAttackRange.h"
#include "MonsterAI.h"
#include "NormalMonster.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BlackBoardKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Player in Attack Range");
}

void UBTService_IsInAttackRange::OnBecomeRelevant(UBehaviorTreeComponent& owner_comp, uint8* node_memory)	
{
	Super::OnBecomeRelevant(owner_comp, node_memory);	

	auto const cont = Cast<AMonsterAI>(owner_comp.GetAIOwner());	
	auto const Monster = Cast<ANormalMonster>(cont->GetPawn());
	ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	float fAttackRange = Monster->GetAttackRange();

	cont->GetBlackBoard()->SetValueAsBool(bb_keys::is_in_attackrange, Monster->GetDistanceTo(Player) <= fAttackRange);
}