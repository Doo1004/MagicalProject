// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IAInterface.h"
#include "Weapons.generated.h"

UCLASS()
class MAGICALPROJECT_API AWeapons : public AActor, public IIAInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapons();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

protected:
	float	m_fRotation;
	bool	m_bOverlap;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* AudioClip;


public:
	USoundBase* GetAudioClip() { return AudioClip; }
	virtual void SetWidget(bool _bActive) override { m_bOverlap = _bActive; };
};
