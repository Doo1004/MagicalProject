#include "MonsterSpawner.h"
#include "Components/CapsuleComponent.h"
#include "../MonsterAI.h"
#include "../NormalMonster.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = true;


	MonsterSpawner = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MonsterSpawner"));
	MonsterSpawner->InitCapsuleSize(42.f, 96.0f);
	RootComponent = MonsterSpawner;

	m_cMonster_1 = LoadClass<ANormalMonster>(NULL, TEXT("/Game/CustomAsset/Blueprint/Monster/Zombie_01_BP.Zombie_01_BP_C"));
	m_cMonster_2 = LoadClass<ANormalMonster>(NULL, TEXT("/Game/CustomAsset/Blueprint/Monster/Zombie_02_BP.Zombie_02_BP_C"));
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

}

void AMonsterSpawner::SpawnMonster()
{
	if (!m_cMonster_1 || !m_cMonster_2)
		return;

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	int iRand = FMath::RandRange(1, 2);

	if (iRand == 1)
		ANormalMonster* Monster = GetWorld()->SpawnActor<ANormalMonster>(m_cMonster_1, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
	else
		ANormalMonster* Monster = GetWorld()->SpawnActor<ANormalMonster>(m_cMonster_2, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
}