// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "NormalMonster.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerProjectile::APlayerProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(50.f);
	RootComponent = SphereComponent;
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerProjectile::OnOverlapBegin);

	// Particle Component
	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile Particle"));
	ProjectileParticle->SetupAttachment(RootComponent);

	// Load Particle(Default)
	static ConstructorHelpers::FObjectFinder<UParticleSystem>FireBall(TEXT("/Game/Particle/FXVarietyPack/Particles/P_ky_fireBall.P_ky_fireBall"));
	MagicAttack = FireBall.Object;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = SphereComponent;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;

	SetLifeSpan(LifeTime);
}

// Called when the game starts or when spawned
void APlayerProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileParticle)
	{
		ProjectileParticle->SetTemplate(MagicAttack);
		ProjectileParticle->Activate(true);
	}
}

// Called every frame
void APlayerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString sOtherName = OtherActor->GetActorLabel();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, sOtherName);

	FVector CollosionLocation = SweepResult.ImpactPoint;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation(), GetActorRotation());
	Destroy();
	//if (sOtherName.Contains(TEXT("Zombie")))
	{

	}
}

void APlayerProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}