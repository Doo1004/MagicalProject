#include "NormalMonster.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MonsterAI.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ANormalMonster::ANormalMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ANormalMonster::OnOverlapBegin);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem>Blood(TEXT("/Game/CustomAsset/Effect/P_Blood"));
	m_HitParticle = Blood.Object;

	HP = 100;
	MaxHP = HP;
}

void ANormalMonster::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = MonsterWalkSpeed;
}

float ANormalMonster::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP < 0.f)
		return 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return 0.f;
}

// Called every frame
void ANormalMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(m_HitParticle)
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, TEXT("ParticleLoaded"));
	//===============================Set Status
	if (HP > 0)
	{
		if (!GetVelocity().IsNearlyZero())	
			m_eMonsterStatus = EMonsterStatus::MOVE;
		else if (GetVelocity().IsNearlyZero() && IsInAttackRange)	
			m_eMonsterStatus = EMonsterStatus::ATTACK;	
		else
			m_eMonsterStatus = EMonsterStatus::IDLE;	
	}

	switch (m_eMonsterStatus)
	{
	case ANormalMonster::EMonsterStatus::IDLE:
		IdleFunction();
		break;
	case ANormalMonster::EMonsterStatus::MOVE:
		MoveFunction();
		break;
	case ANormalMonster::EMonsterStatus::ATTACK:
		AttackFunction();	
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, TEXT("Monster Error"));
		break;
	}

	DefaultFunction(DeltaTime);	
}

void ANormalMonster::MonsterAttack()
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	AttackType = FMath::RandRange(1, 3);	
	FString MontageName = FString::Printf(TEXT("Attack_%d"), AttackType);	
	AnimInst->Montage_Play(UpperMontage, 1.f);	
	AnimInst->Montage_JumpToSection(FName(MontageName), UpperMontage);
	FOnMontageEnded Delegate;
	Delegate.BindUObject(this, &ANormalMonster::ClearAttackNum);
	AnimInst->Montage_SetEndDelegate(Delegate, UpperMontage);
}

void ANormalMonster::IdleFunction()
{
	if (IdleType == 0)
		IdleType = FMath::RandRange(1, 2);

	AttackType = 0;
	MoveType = 0;
}

void ANormalMonster::MoveFunction()
{
	if (m_bIsPlayerDetected && MoveType == 0 ||
		m_bIsPlayerDetected && MoveType == 3)
		MoveType = FMath::RandRange(1, 2);
	else if (!m_bIsPlayerDetected && MoveType == 0)
		MoveType = 3;

	IdleType = 0;
	AttackType = 0;
}

void ANormalMonster::AttackFunction()
{
	MoveType = 0;
	IdleType = 0;
}

void ANormalMonster::DefaultFunction(float _DT)
{
	AMonsterAI* MonsterAI = Cast<AMonsterAI>(GetController());
	if (MonsterAI)
	{
		m_bIsPlayerDetected = MonsterAI->IsPlayerDetected();
		IsInAttackRange = MonsterAI->IsInAttackRange();
	}

	HP = FMath::Clamp(HP, 0.f, MaxHP);
}

void ANormalMonster::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!m_bIsDead)	
	{
		FString sOtherName = OtherComp->GetOwner()->GetName(); 
		int iOtherNum = 0;
		FVector HitPoint = (OtherComp->GetComponentLocation() + GetActorLocation()) / 2;

		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, OtherComp->GetName());
		//FString Message1 = FString::Printf(TEXT("HP : %f"), HP);	
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Message1);

		if (OtherComp->GetName().Contains(TEXT("LeftHand")))		{ HP -= 10.f; iOtherNum = 1; }	
		else if (OtherComp->GetName().Contains(TEXT("RightHand")))	{ HP -= 15.f; iOtherNum = 2; }
		else if (sOtherName.Contains(TEXT("WoodStick")))			{ HP -= 26.f; iOtherNum = 3; }
		else if (sOtherName.Contains(TEXT("Hammer")))				{ HP -= 34.f; iOtherNum = 4; }
		else if (sOtherName.Contains(TEXT("Knife")))				{ HP -= 65.f; iOtherNum = 5; }
		else if (sOtherName.Contains(TEXT("FireBall")))				{ HP -= 70.f; iOtherNum = 6; }			
		else if (sOtherName.Contains(TEXT("ElectricBall")))			{ HP -= 80.f; iOtherNum = 7; }	

		if (HP > 0.f && iOtherNum != 0)
			HitReaction(iOtherNum, HitPoint);
		else if (HP <= 0.f)
		{
			m_vImpactDirection = GetImpactDirection(OtherActor, this);		
			m_fImpactStrength = CalculateImpactStrength(OtherActor, iOtherNum);	
			m_vImpulse = m_vImpactDirection * m_fImpactStrength;	
			DeadReaction(iOtherNum, HitPoint);
		}
	}
}

void ANormalMonster::ClearAttackNum(UAnimMontage* Montage, bool bInterrupted)
{
	AttackType = 0;
}

void ANormalMonster::HitReaction(int _WeaponNum, FVector _HitPoint)
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();	
	AnimInst->Montage_Play(UpperMontage, 1.f);
	AnimInst->Montage_JumpToSection(FName("HitReact"), UpperMontage);

	// ...Particle & SoundPlay
	if (_WeaponNum == 1 || _WeaponNum == 2)
		AudioComponent->SetSound(PunchHitSound);
	else if (_WeaponNum == 3 || _WeaponNum == 4)
		AudioComponent->SetSound(ClubHitSound);
	else if (_WeaponNum == 5)
		AudioComponent->SetSound(KnifeHitSound);
	else if (_WeaponNum == 6 || _WeaponNum == 7)
		AudioComponent->SetSound(ProjectileHitSound);	

	AudioComponent->Play();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_HitParticle, _HitPoint, GetActorRotation());
}

void ANormalMonster::DeadReaction(int _WeaponNum, FVector _HitPoint)
{
	// ...Particle & SoundPlay
	if (_WeaponNum == 1 || _WeaponNum == 2)	
		AudioComponent->SetSound(PunchHitSound);	
	else if (_WeaponNum == 3 || _WeaponNum == 4)	
		AudioComponent->SetSound(ClubHitSound);	
	else if (_WeaponNum == 5)	
		AudioComponent->SetSound(KnifeHitSound);	
	else if (_WeaponNum == 6 || _WeaponNum == 7)	
		AudioComponent->SetSound(ProjectileHitSound);		

	AudioComponent->Play();	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_HitParticle, _HitPoint, GetActorRotation());	


	m_bIsDead = true;
	
	if(GetController())
		GetController()->UnPossess();

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();	

	if (_WeaponNum >= 1 && _WeaponNum <= 5)
	{
		int DeathNum = FMath::RandRange(1, 2);
		AnimInst->Montage_Play(FullMontage, 1.f);
		FString MontageName;

		if (CalculateImpactDirection() == 0)
			MontageName = FString::Printf(TEXT("DeathForward_0%d"), DeathNum);
		else if (CalculateImpactDirection() == 1)
			MontageName = FString::Printf(TEXT("DeathBack_0%d"), DeathNum);
		else if (CalculateImpactDirection() == 2)
			MontageName = FString::Printf(TEXT("DeathLeft_0%d"), DeathNum);
		else if (CalculateImpactDirection() == 3)
			MontageName = FString::Printf(TEXT("DeathRight_0%d"), DeathNum);

		AnimInst->Montage_JumpToSection(FName(MontageName), FullMontage);

		FString Name2 = FString::Printf(TEXT("Number : %d"), CalculateImpactDirection());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Name2);
	}
	else if (_WeaponNum >= 6 && _WeaponNum <= 7)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
		GetMesh()->SetSimulatePhysics(true);	
		GetMesh()->SetAllBodiesSimulatePhysics(true);	
		GetMesh()->WakeAllRigidBodies();	
		GetMesh()->AddImpulse(m_vImpulse, NAME_None, true);	
		SetLifeSpan(5.f);
	}
}

void ANormalMonster::ActivePhysics()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);		
	GetMesh()->SetAllBodiesSimulatePhysics(true);		
	GetMesh()->WakeAllRigidBodies();		
	SetLifeSpan(5.f);
}

FVector ANormalMonster::GetImpactDirection(AActor* DamageCauser, AActor* DamagedActor)
{
	FVector ImpactDirection = DamagedActor->GetActorLocation() - DamageCauser->GetActorLocation();
	ImpactDirection.Normalize();
	return ImpactDirection;
}

float ANormalMonster::CalculateImpactStrength(AActor* DamageCauser, int ProjectileNum)
{
	float ImpactStrength;

	if (ProjectileNum == 1) { ImpactStrength = 1500.f; }			// Test
	else if (ProjectileNum == 2) { ImpactStrength = 1700.f; }		// Test
	else if (ProjectileNum == 3) { ImpactStrength = 2500.f; }		// Test
	else if (ProjectileNum == 4) { ImpactStrength = 3000.f; }		// Test
	else if (ProjectileNum == 6) { ImpactStrength = 5000.f; }
	else if (ProjectileNum == 7) { ImpactStrength = 6000.f; }
	else { ImpactStrength = 0.f; }

	return ImpactStrength;	
}

int32 ANormalMonster::CalculateImpactDirection()
{
	FVector Forward = FVector(1, 0, 0);
	FVector Backward = FVector(-1, 0, 0);
	FVector Left = FVector(0, -1, 0);
	FVector Right = FVector(0, 1, 0);
	
	float ForwardDot = FVector::DotProduct(m_vImpactDirection, Forward);
	float BackwardDot = FVector::DotProduct(m_vImpactDirection, Backward);
	float LeftDot = FVector::DotProduct(m_vImpactDirection, Left);
	float RightDot = FVector::DotProduct(m_vImpactDirection, Right);
	float Dot1 = FMath::Max(ForwardDot, BackwardDot);
	float Dot2 = FMath::Max(LeftDot, RightDot);
	float MaxDot = FMath::Max(Dot1, Dot2);

	if (MaxDot == ForwardDot)		return 0;
	else if (MaxDot == BackwardDot)	return 1;
	else if (MaxDot == LeftDot)		return 2;
	else if (MaxDot == RightDot)	return 3;
	else							return 0;
}