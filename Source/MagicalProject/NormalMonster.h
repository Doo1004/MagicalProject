// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterAI.h"
#include "NormalMonster.generated.h"

UCLASS()
class MAGICALPROJECT_API ANormalMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ANormalMonster();

public:
	//=============================Monster State With Blueprint=============================//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	float MonsterWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	float AttackPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	float AttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "Pawn")
	float MaxHP;
	UPROPERTY(BlueprintReadOnly, Category = "Pawn")
	int	MoveType;
	UPROPERTY(BlueprintReadOnly, Category = "Pawn")
	int AttackType;
	UPROPERTY(BlueprintReadOnly, Category = "Pawn")
	int IdleType;
	UPROPERTY(BlueprintReadOnly, Category = "Pawn")	
	bool IsInAttackRange;

	//====================================Anim Montage=====================================//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")	
	UAnimMontage* UpperMontage;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	UAnimMontage* FullMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundBase* KnifeHitSound;
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundBase* ClubHitSound;
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundBase* PunchHitSound;
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundBase* ProjectileHitSound;

protected:
	enum class EMonsterStatus
	{
		IDLE,
		MOVE,
		ATTACK,
	};

	EMonsterStatus		m_eMonsterStatus;
	
	float				m_fPlayerDistance;
	bool				m_bIsPlayerDetected;
	bool				m_bIsDead;

	FVector				m_vImpulse;
	FVector				m_vImpactDirection;
	float				m_fImpactStrength;

	AMonsterAI*			m_MonsterAI;
	UParticleSystem*	m_HitParticle;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void IdleFunction();
	void MoveFunction();
	void AttackFunction();
	void DefaultFunction(float _DT);
	void ClearAttackNum(UAnimMontage* Montage, bool bInterrupted);

	void HitReaction(int _WeaponNum, FVector _HitPoint);
	void DeadReaction(int _WeaponNum, FVector _HitPoint);

	FVector GetImpactDirection(AActor* DamageCauser, AActor* DamagedActor);
	float CalculateImpactStrength(AActor* DamageCauser, int ProjectileNum);
	int32 CalculateImpactDirection();

public:	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Function")
	void ActivePhysics();

	virtual void Tick(float DeltaTime) override;
	float GetAttackRange() { return AttackRange; }
	void MonsterAttack();
	
	bool IsMonsterDead() { return m_bIsDead; }
};
