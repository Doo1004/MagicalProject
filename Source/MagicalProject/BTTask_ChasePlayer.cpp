// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "BlackBoardKeys.h"
#include "MonsterAI.h"
 
UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Chase Player");
}


EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* nodememory)
{
	AMonsterAI* const cont = Cast<AMonsterAI>(owner_comp.GetAIOwner());
	ANormalMonster* const Monster = Cast<ANormalMonster>(cont->GetPawn());
	FVector const vPlayerLocation = cont->GetBlackBoard()->GetValueAsVector(bb_keys::target_location);

	if(!IsMontagePlaying(Monster))	
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(cont, vPlayerLocation);

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);	
	return EBTNodeResult::Succeeded;	
}

bool UBTTask_ChasePlayer::IsMontagePlaying(ANormalMonster* const monster)
{
	return monster->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
}
