// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerProjectile.generated.h"

UCLASS()
class MAGICALPROJECT_API APlayerProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* ProjectileParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UParticleSystem* MagicAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	float LifeTime;	

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void FireInDirection(const FVector& ShootDirection);

};
