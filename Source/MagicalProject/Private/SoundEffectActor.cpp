// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundEffectActor.h"

// Sets default values
ASoundEffectActor::ASoundEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoundEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

