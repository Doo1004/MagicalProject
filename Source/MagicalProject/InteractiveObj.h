// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SpotLightComponent.h"	
#include "Components/AudioComponent.h"
#include "IAInterface.h"
#include "Sound/SoundWave.h"	
#include "InteractiveObj.generated.h"

UENUM(BlueprintType)
enum class EInteractiveObjectType : uint8
{
	Door		UMETA(DisplayName = "Door"),
	Switch		UMETA(DisplayName = "Switch"),
	Trigger		UMETA(DisplayName = "Trigger"),
};

UCLASS()
class MAGICALPROJECT_API AInteractiveObj : public AActor, public IIAInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveObj();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DoorFunction(float _DT);
	void SwitchFunction();
	void TriggerFunction(float _DT);
	void WidgetFunction();

	void SetComponents();
	void LinkTriggerObj();
	void LightActive();
	void LightNoise();
	void LightChangeMaterial();
	void BulbSoundPlay();

	bool m_bTrigger;
	bool m_bOverlap;
	bool m_bSoundPlay;

	float m_fDotProduct;
	float m_fOpenDoorAngle;
	float m_fMaxTurnOnDelay;


	//=================================Components & Actor
	TArray<UActorComponent*>	m_aComponents;	
	ULightComponent*			m_cLight;
	TArray<AInteractiveObj*>	m_aTriggerActors;
	USoundWave*					m_uLockedSound;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* ActiveWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* DeactiveWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")	
	USoundBase* AudioClip;	

	UPROPERTY(EditAnywhere, Category = "ObjectInfo")
	EInteractiveObjectType ObjectType;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	float OpenDoorAngle;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	float OpenDoorSpeed;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	bool IsDoorForward;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	bool IsSwingDoor;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	bool IsLocked;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	USoundWave* DoorOpenSound;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	USoundWave* DoorCloseSound;

	UPROPERTY(EditAnywhere, Category = "DoorInfo")
	FString NeedKeyName;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool AutoTrigger;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	float TurnOnDelay;

	UPROPERTY(EditAnywhere, Category = "Trigger", meta = (DisplayName = "Use Light Noise"))
	bool UseLightNoise;

	UPROPERTY(EditAnywhere, Category = "Trigger", meta = (EditCondition = "Use Light Noise", DisplayName = "MinNoiseTime"))
	float MinNoiseTime;

	UPROPERTY(EditAnywhere, Category = "Trigger", meta = (EditCondition = "Use Light Noise", DisplayName = "MaxNoiseTime"))
	float MaxNoiseTime;

	UPROPERTY(EditAnywhere, Category = "Switch")
	bool Active;

	UPROPERTY(EditAnywhere, Category = "Material")
	int MaterialNumber;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* BulbOnMaterial;	

	UPROPERTY(EditAnywhere, Category = "Material")	
	UMaterialInterface* BulbOffMaterial;	

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ActiveObject();
	virtual void SetWidget(bool _bActive) override { m_bOverlap = _bActive; };
	void OpenLockedDoor();
	void LockedSoundPlay();
};
