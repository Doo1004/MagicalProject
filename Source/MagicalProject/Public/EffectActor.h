// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "EffectActor.generated.h"

UCLASS()
class MAGICALPROJECT_API AEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEffectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AudioFunction();
	void ParticleFunction();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UAudioComponent* AudioComponent;
};
