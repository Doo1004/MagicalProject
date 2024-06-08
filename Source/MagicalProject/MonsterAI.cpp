// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "BlackBoardKeys.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MagicalProjectCharacter.h"

AMonsterAI::AMonsterAI(FObjectInitializer const& object_initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>obj(TEXT("/Game/CustomAsset/AI/MonsterAI_BT"));

	if (obj.Succeeded())
		BehaviorTree = obj.Object;

	Behavior_Tree_Comp = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComponent"));
	Blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight_Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfig->SightRadius = 500.f;
	SightConfig->LoseSightRadius = 550.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAI::OnTargetDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AMonsterAI::BeginPlay()
{
	Super::BeginPlay();

	//SightConfig->SightRadius = AISightRadius;
	//SightConfig->LoseSightRadius = AILoseSightRadius;
	//SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;	
	//SightConfig->SetMaxAge(AISightAge);
	//
	//RunBehaviorTree(BehaviorTree);
	//Behavior_Tree_Comp->StartTree(*BehaviorTree);
}

void AMonsterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsPlayerDetected = GetBlackBoard()->GetValueAsBool(bb_keys::is_find_player);
	vPlayerLocation = GetBlackBoard()->GetValueAsVector(bb_keys::target_location);
	bIsInAttackRange = GetBlackBoard()->GetValueAsBool(bb_keys::is_in_attackrange);
}

void AMonsterAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	RunBehaviorTree(BehaviorTree);
	Behavior_Tree_Comp->StartTree(*BehaviorTree);

	if (Blackboard)
		Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
}

FRotator AMonsterAI::GetControlRotation() const
{
	if (GetPawn() == nullptr)
		return FRotator(0.f, 0.f, 0.f);

	return FRotator(0.f, GetPawn()->GetActorRotation().Yaw, 0.f);
}

void AMonsterAI::OnTargetDetected(AActor* _Actor, FAIStimulus const _Stimulus)
{
	if (auto const Player = Cast<AMagicalProjectCharacter>(_Actor))
		GetBlackBoard()->SetValueAsBool(bb_keys::is_find_player, _Stimulus.WasSuccessfullySensed());
}
