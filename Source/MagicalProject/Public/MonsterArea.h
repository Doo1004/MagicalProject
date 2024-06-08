// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.h"
#include "../MonsterAI.h"
#include "MonsterArea.generated.h"

UCLASS()
class MAGICALPROJECT_API AMonsterArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterArea();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	void LinkSpawner();
	bool FindPlayer();
	int32 NeedMonsterCount();
	void SpawnMonster();

	float						m_fSpawnTimer;
	TArray<AActor*>				m_aOverlappingActors;
	TArray<AMonsterSpawner*>	m_aSpawnerArray;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* MonsterArea;

	UPROPERTY(EditAnywhere, Category = "SpawnTimer")
	float MonsterSpawnTime;

};
