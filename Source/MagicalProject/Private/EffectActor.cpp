// Fill out your copyright notice in the Description page of Project Settings.
#include "EffectActor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AEffectActor::AEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(10.f);
	RootComponent = SphereComponent;

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
} 

// Called when the game starts or when spawned
void AEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(AudioComponent->IsPlaying())
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, TEXT("Sound Playing"));

	AudioFunction();
	ParticleFunction();
}

void AEffectActor::AudioFunction()
{
	if (AudioComponent)
		AudioComponent->Play();
}

void AEffectActor::ParticleFunction()
{
	if (ParticleSystem)
		ParticleSystem->Activate(true);
}