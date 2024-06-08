// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/WidgetComponent.h"

// Sets default values
AWeapons::AWeapons()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));	
	SphereComponent->InitSphereRadius(100.f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapons::OnOverlapBegin);	
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWeapons::OnOverlapEnd);

	RootComponent = SphereComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Obj"));
	StaticMesh->SetupAttachment(RootComponent);


	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponWidget"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawAtDesiredSize(true);
	WidgetComponent->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<USoundBase>AudioAsset(TEXT("/Game/StarterContent/Audio/Explosion02.Explosion02"));

	AudioClip = AudioAsset.Object;
}

// Called when the game starts or when spawned
void AWeapons::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WidgetComponent)
		WidgetComponent->SetVisibility(m_bOverlap);

	m_fRotation += DeltaTime * 70.f;

	if (m_fRotation >= 360.f)
		m_fRotation = 0.f;

	FRotator ItemRotation(0.f, m_fRotation, 25.f);
	SetActorRotation(ItemRotation);
}

void AWeapons::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//FString sOtherName = OtherComp->GetOwner()->GetName();	
	//
	//if (sOtherName.Contains(TEXT("Doori")))	
	//	WidgetComponent->SetVisibility(true);	
}

void AWeapons::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//FString sOtherName = OtherComp->GetOwner()->GetName();
	//
	//if (sOtherName.Contains(TEXT("Doori")))
	//	WidgetComponent->SetVisibility(false);
}