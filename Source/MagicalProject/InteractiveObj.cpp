// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveObj.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"	
#include "Components/WidgetComponent.h"	
#include "Components/SpotLightComponent.h"	
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInteractiveObj::AInteractiveObj()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveObj::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractiveObj::OnOverlapEnd);
	RootComponent = BoxComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Obj"));
	StaticMesh->SetupAttachment(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;

	ActiveWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActiveWidget"));
	ActiveWidget->SetupAttachment(StaticMesh);
	ActiveWidget->SetWidgetSpace(EWidgetSpace::Screen);	
	ActiveWidget->SetDrawAtDesiredSize(true);	
	ActiveWidget->SetVisibility(false);

	DeactiveWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeactiveWidget"));
	DeactiveWidget->SetupAttachment(StaticMesh);
	DeactiveWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DeactiveWidget->SetDrawAtDesiredSize(true);
	DeactiveWidget->SetVisibility(false);

	if (ObjectType == EInteractiveObjectType::Door)
	{
		ConstructorHelpers::FObjectFinder<USoundWave>LockedSound(TEXT("/Game/CustomAsset/Sound/Lock"));
		m_uLockedSound = LockedSound.Object;
	}

	OpenDoorAngle = 110.f;
	OpenDoorSpeed = 3.f;
}

void AInteractiveObj::BeginPlay()
{
	Super::BeginPlay();

	if (ObjectType == EInteractiveObjectType::Switch)
		LinkTriggerObj();
	else if (ObjectType == EInteractiveObjectType::Trigger)
		SetComponents();

	m_fMaxTurnOnDelay = TurnOnDelay;
}

void AInteractiveObj::Tick(float DeltaTime)	
{
	Super::Tick(DeltaTime);

	if (ObjectType == EInteractiveObjectType::Door)
	{
		if (AudioComponent->IsPlaying())
			GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, TEXT("SoundPlay"));
		else
			GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, TEXT("SoundStopped"));
	}

	//if (AutoTrigger)
	//{
	//	FString Message1 = FString::Printf(TEXT("TriggerOn : %d"), Active);
	//	GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, Message1);
	//}

	switch (ObjectType)
	{	
	case EInteractiveObjectType::Door:	
		DoorFunction(DeltaTime);
		break;	
	case EInteractiveObjectType::Switch:	
		SwitchFunction();	
		break;
	case EInteractiveObjectType::Trigger:	
		TriggerFunction(DeltaTime);
		break;
	}

	WidgetFunction();
}

void AInteractiveObj::OpenLockedDoor()
{
	IsLocked = false;

	// Sound Play
}

void AInteractiveObj::DoorFunction(float _DT)
{
	if (IsLocked)
		return;

	if (Active)
	{
		if (DoorOpenSound && m_bSoundPlay)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DoorOpenSound, GetActorLocation());
			m_bSoundPlay = false;
		}

		if (IsSwingDoor)
		{
			if (m_bTrigger)	// 한 번만 실행
			{
				FVector vPlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
				FVector vToCharVector = vPlayerLocation - GetActorLocation();
				vToCharVector.Normalize();

				if (IsDoorForward)
					m_fDotProduct = FVector::DotProduct(vToCharVector, GetActorForwardVector());
				else
					m_fDotProduct = FVector::DotProduct(vToCharVector, GetActorRightVector());

				m_bTrigger = false;
			}
			if (m_fDotProduct > 0)
				m_fOpenDoorAngle = FMath::FInterpTo(m_fOpenDoorAngle, -OpenDoorAngle, _DT, OpenDoorSpeed);
			else
				m_fOpenDoorAngle = FMath::FInterpTo(m_fOpenDoorAngle, OpenDoorAngle, _DT, OpenDoorSpeed);
		}
		else
			m_fOpenDoorAngle = FMath::FInterpTo(m_fOpenDoorAngle, OpenDoorAngle, _DT, OpenDoorSpeed);
	}
	else
	{
		if (DoorCloseSound && m_bSoundPlay)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DoorCloseSound, GetActorLocation());
			m_bSoundPlay = false;
		}

		m_fOpenDoorAngle = FMath::FInterpTo(m_fOpenDoorAngle, 0.f, _DT, OpenDoorSpeed);
	}

	StaticMesh->SetRelativeRotation(FRotator(0.f, m_fOpenDoorAngle, 0.f));
}

void AInteractiveObj::SwitchFunction()
{
	if (Active)	
	{
		for (AInteractiveObj* Actor : m_aTriggerActors)	
		{	
			if (Actor)	
				Actor->ActiveObject();	
		}

		if (!AudioComponent->IsPlaying())	
			AudioComponent->Play();	

		Active = false;	
	}
}

void AInteractiveObj::TriggerFunction(float _DT)
{
	if (Active)
	{
		if (m_bTrigger)
			TurnOnDelay -= _DT;

		if (TurnOnDelay <= 0.f)
		{
			m_bTrigger = false;
			LightActive();
			TurnOnDelay = m_fMaxTurnOnDelay;
		}
	}
	else
		LightActive();

	LightChangeMaterial();
	BulbSoundPlay();
}

void AInteractiveObj::WidgetFunction()
{
	if (!ActiveWidget || !DeactiveWidget)
		return;

	if (AutoTrigger)
		return;

	if (m_bOverlap)
	{
		if (Active)
		{
			ActiveWidget->SetVisibility(true);
			DeactiveWidget->SetVisibility(false);
		}
		else
		{
			ActiveWidget->SetVisibility(false);
			DeactiveWidget->SetVisibility(true);
		}
	}
	else
	{
		ActiveWidget->SetVisibility(false);
		DeactiveWidget->SetVisibility(false);
	}
}

void AInteractiveObj::SetComponents()
{
	GetComponents(m_aComponents);

	for (auto* Comp : m_aComponents)
	{
		if (m_cLight)
			break;
		m_cLight = Cast<USpotLightComponent>(Comp);
		if(!m_cLight)
			m_cLight = Cast<UPointLightComponent>(Comp);

		//...Add Components
	}

	//==================Components Setting
	if (m_cLight)
		m_cLight->SetVisibility(Active);
}

void AInteractiveObj::LightActive()
{
	if (m_cLight)
	{
		m_cLight->SetVisibility(Active);	
		LightNoise();
	}
}

void AInteractiveObj::LightNoise()	
{
	if (UseLightNoise && Active)
	{
		m_cLight->SetVisibility(!m_cLight->IsVisible());

		FTimerHandle TimerHandle_LightNoise;
		float NoiseTime = FMath::RandRange(MinNoiseTime, MaxNoiseTime);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_LightNoise, this, &AInteractiveObj::LightNoise, NoiseTime, false);
	}
}

void AInteractiveObj::LightChangeMaterial()
{
	if (m_cLight)
	{
		if (m_cLight->IsVisible())
			StaticMesh->SetMaterial(MaterialNumber, BulbOnMaterial);
		else
			StaticMesh->SetMaterial(MaterialNumber, BulbOffMaterial);
	}
}

void AInteractiveObj::BulbSoundPlay()
{
	if (AudioComponent && m_cLight)
	{
		if (m_cLight->IsVisible())
		{
			if (!AudioComponent->IsPlaying())
				AudioComponent->Play();
		}
		else
		{
			if (AudioComponent->IsPlaying())
				AudioComponent->Stop();
		}
	}
}

void AInteractiveObj::LockedSoundPlay()
{
	if(m_uLockedSound)
		UGameplayStatics::PlaySoundAtLocation(this, m_uLockedSound, GetActorLocation());
}

void AInteractiveObj::ActiveObject()
{
	if (Active)
		Active = false;
	else
		Active = true;

	m_bTrigger = true;
	m_bSoundPlay = true;
}

void AInteractiveObj::LinkTriggerObj()
{
	// Switch : IASwitch_(Num...)
	// Trigger : IATrigger_(Num...)_ActorType

	FString Number;
	GetActorLabel().Split(TEXT("_"), nullptr, &Number);
	FString TriggerName;
	TriggerName = TEXT("IATrigger_") + Number;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractiveObj::StaticClass(), FoundActors);

	m_aTriggerActors.Empty();

	for (auto Actor : FoundActors)
	{
		if (Actor->GetActorLabel().Contains(TriggerName))
		{
			AInteractiveObj* FindIAObj = Cast<AInteractiveObj>(Actor);
			if (FindIAObj)
				m_aTriggerActors.Add(FindIAObj);
		}
	}
}

void AInteractiveObj::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AutoTrigger)
		return;

	FString sOtherName = OtherComp->GetOwner()->GetName();	

	if (sOtherName.Contains(TEXT("Doori")))
		Active = true;
}

void AInteractiveObj::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!AutoTrigger)
		return;

	FString sOtherName = OtherComp->GetOwner()->GetName();	

	if (sOtherName.Contains(TEXT("Doori")))
		Active = false;
}

