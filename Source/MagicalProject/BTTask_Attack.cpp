// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "MonsterAI.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackBoardKeys.h"

UBTTask_Attack::UBTTask_Attack(FObjectInitializer const& object_initializer)
{

}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* nodememory)
{
	AMonsterAI* const cont = Cast<AMonsterAI>(owner_comp.GetAIOwner());
	ANormalMonster* const Monster = Cast<ANormalMonster>(cont->GetPawn());

	bool IsPlayerInRange = cont->GetBlackBoard()->GetValueAsBool(bb_keys::is_in_attackrange);
	if (IsPlayerInRange && !IsMontagePlaying(Monster))
		Monster->MonsterAttack();

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

bool UBTTask_Attack::IsMontagePlaying(ANormalMonster* const monster)	
{
	return monster->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
}
