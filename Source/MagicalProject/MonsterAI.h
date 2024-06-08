// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAI.generated.h"

/**
 * 
 */
class UBehaviorTree;	
class UBlackboardComponent;	

UCLASS()

class MAGICALPROJECT_API AMonsterAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AMonsterAI(FObjectInitializer const& object_initializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual FRotator GetControlRotation() const override;
	bool IsPlayerDetected() { return bIsPlayerDetected; }
	bool IsInAttackRange() { return bIsInAttackRange; }


	class UBlackboardComponent* GetBlackBoard() const { return Blackboard; }

	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void OnTargetDetected(AActor* _Actor, FAIStimulus const _Stimulus);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AISightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AISightAge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AILoseSightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AIFieldOfView;

	UPROPERTY(transient)
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTreeComponent* Behavior_Tree_Comp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* Bhavior_Tree;

	class UAISenseConfig_Sight* SightConfig;
	class UBlackboardComponent* Blackboard;

protected:
	bool						bIsPlayerDetected;
	bool						bIsInAttackRange;
	FVector						vPlayerLocation;
};
