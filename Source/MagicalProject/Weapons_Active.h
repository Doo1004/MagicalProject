// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons_Active.generated.h"

UCLASS()
class MAGICALPROJECT_API AWeapons_Active : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapons_Active();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float m_fDurability;
	float m_fMaxDurability;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponsData")
	float Damage;

	bool IsDurabilityIsZero() { return m_fDurability <= 0.f; }

	float GetDurability() { return m_fDurability; }
	float GetMaxDurability() { return m_fMaxDurability; }

	void SetDurability(float _durability) { m_fDurability = _durability; }
	void SetMaxDurability(float _maxdurability) { m_fMaxDurability = _maxdurability; }
	void SetActiveBoxComponent(bool _enable);
};
