// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons_Active.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"

// Sets default values
AWeapons_Active::AWeapons_Active()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Obj"));
	RootComponent = StaticMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapons_Active::OnOverlapBegin);
	BoxComponent->SetupAttachment(RootComponent);

	m_fDurability = 100.f;
}

// Called when the game starts or when spawned
void AWeapons_Active::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void AWeapons_Active::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//m_fDurability = FMath::Min(m_fDurability, 0);
}

void AWeapons_Active::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetActorLabel().Contains(TEXT("Zombie")))
	{
		m_fDurability -= 1;	

		if (m_fDurability <= 0.f)	
			Destroy();	
	}
}

void AWeapons_Active::SetActiveBoxComponent(bool _enable)
{
	if (_enable)
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	else
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

