#include "MonsterArea.h"
#include "../NormalMonster.h"
#include "Components/BoxComponent.h"
#include "../MagicalProjectCharacter.h"
#include "MonsterSpawner.h"
#include "Kismet/GameplayStatics.h"

AMonsterArea::AMonsterArea()
{
	PrimaryActorTick.bCanEverTick = true;

	MonsterArea = CreateDefaultSubobject<UBoxComponent>(TEXT("MonsterArea"));
	RootComponent = MonsterArea;

	MonsterSpawnTime = 3.f;
}

void AMonsterArea::BeginPlay()
{
	Super::BeginPlay();
	LinkSpawner();
	m_fSpawnTimer = MonsterSpawnTime;
}

void AMonsterArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetOverlappingActors(m_aOverlappingActors);

	//FString Message1 = FString::Printf(TEXT("SpawnTime : %f"), m_fSpawnTimer);
	//GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, Message1);
	//FString Message2 = FString::Printf(TEXT("FindPlayer : %d"), FindPlayer());
	//GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, Message2);
	//FString Message3 = FString::Printf(TEXT("NeedMonster : %d"), NeedMonsterCount());
	//GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, Message3);
	//FString Message4 = FString::Printf(TEXT("OverlappingActors : %d"), m_aOverlappingActors.Num());
	//GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, Message4);

	if (FindPlayer())
	{
		m_fSpawnTimer -= DeltaTime;
		if (m_fSpawnTimer <= 0.f)
		{
			SpawnMonster();
			m_fSpawnTimer = MonsterSpawnTime;
		}
	}
	else
		m_fSpawnTimer = MonsterSpawnTime;
}

void AMonsterArea::SpawnMonster()
{
	// Shuffle SpawnerArray
	for (int32 Idx = 0; Idx < m_aSpawnerArray.Num(); Idx++)
	{
		int32 SwapIdx = FMath::RandRange(0, Idx);
		m_aSpawnerArray.Swap(Idx, SwapIdx);
	}

	for (int32 i = 0; i < NeedMonsterCount(); i++)
	{
		m_aSpawnerArray[i]->SpawnMonster();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, m_aSpawnerArray[i]->GetActorLabel());
	}
}

void AMonsterArea::LinkSpawner()
{
	FString Number;
	GetActorLabel().Split(TEXT("_"), nullptr, &Number);
	FString SpawnerName;
	SpawnerName = TEXT("Spawner") + Number;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundActors);

	m_aSpawnerArray.Empty();

	for (auto Actor : FoundActors)
	{
		if (Actor->GetActorLabel().Contains(SpawnerName))
		{
			AMonsterSpawner* FindSpawner = Cast<AMonsterSpawner>(Actor);
			if (FindSpawner)
				m_aSpawnerArray.Add(FindSpawner);
		}
	}
}

bool AMonsterArea::FindPlayer()
{
	AMagicalProjectCharacter* Player = nullptr;

	for (auto Actor : m_aOverlappingActors)
		Player = Cast<AMagicalProjectCharacter>(Actor);

	if (Player)
		return true;
	else
		return false;
}

int32 AMonsterArea::NeedMonsterCount()
{
	TArray<ANormalMonster*> MonsterArray;

	for (auto Actor : m_aOverlappingActors)
	{
		ANormalMonster* Monster = Cast<ANormalMonster>(Actor);

		if (Monster)
			MonsterArray.Add(Monster);
	}

	if (MonsterArray.Num() < 4)
		return 4 - MonsterArray.Num();
	else
		return 0;
}
