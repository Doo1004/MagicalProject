// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class MAGICALPROJECT_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterSpawner();

	void SpawnMonster();

protected:
	virtual void BeginPlay() override;

	UClass* m_cMonster_1;
	UClass* m_cMonster_2;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* MonsterSpawner;
};
