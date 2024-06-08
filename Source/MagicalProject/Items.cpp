// Fill out your copyright notice in the Description page of Project Settings.


#include "Items.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItems::AItems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//StaticMesh->SetupAttachment(RootComponent);
	RootComponent = StaticMesh;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(100.f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItems::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItems::OnOverlapEnd);
	SphereComponent->SetupAttachment(RootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawAtDesiredSize(true);
	WidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AItems::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WidgetComponent)
		WidgetComponent->SetVisibility(m_bOverlap);

	//if (ItemData.ItemType == EItemType::CONSUMABLE)
	//{
	//	float fSineWave = FMath::Sin(GetWorld()->GetTimeSeconds() * 3.f);
	//	m_fItemPos = fSineWave * 0.3f;
	//	FVector vLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + m_fItemPos);
	//
	//	SetActorLocation(vLocation);
	//}
	//else if (ItemData.ItemType == EItemType::WEAPON)
	//{
	//	m_fItemPos += DeltaTime * 70.f;
	//
	//	if (m_fItemPos >= 360.f)
	//		m_fItemPos = 0.f;
	//
	//	FRotator ItemRotation(0.f, m_fItemPos, 25.f);
	//	SetActorRotation(ItemRotation);
	//}
}


void AItems::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//FString sOtherName = OtherComp->GetOwner()->GetName();
	//
	//if (sOtherName.Contains(TEXT("Doori")))
	//	if(WidgetComponent != nullptr)
	//		WidgetComponent->SetVisibility(true);
}

void AItems::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//FString sOtherName = OtherComp->GetOwner()->GetName();
	//
	//if (sOtherName.Contains(TEXT("Doori")))
	//	if (WidgetComponent != nullptr)
	//		WidgetComponent->SetVisibility(false);
}
