// Copyright Epic Games, Inc. All Rights Reserved.

#include "MagicalProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerProjectile.h"
#include "InteractiveObj.h"
#include "Items.h"
#include "Weapons_Active.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Blueprint/UserWidget.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMagicalProjectCharacter

AMagicalProjectCharacter::AMagicalProjectCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;	
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Stimulus->CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	//Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	//Stimulus->RegisterWithPerceptionSystem();

	m_CastingAimWidget = nullptr;
	m_InventoryWidget = nullptr;

	MoveEnable = true; 
	m_ePlayerStatus = EPlayerStatus::IDLE;
	HP = 100.f;
	MP = 100.f;
	Stamina = 100.f;
	m_bSPRecovery = true;
	m_bMPRecovery = true;

	m_fCastingNeedMP1 = 40.f;
	m_fCastingNeedMP2 = 60.f;
	m_fCastingNeedMP9 = 80.f;

	iWeaponEquipSlotNum = -1;

	ItemSlot.Empty();
	QuickSlotIndex.Empty();

	InitialValueSlot.ItemName = NAME_None;
	InitialValueSlot.Amount = 0;
	InitialValueSlot.MaxAmount = 0;
	InitialValueSlot.ItemType = EItemType::DEFAULT;
	InitialValueSlot.IsEquipped = false;

	for (int32 i = 0; i < 15; ++i)
		ItemSlot.Add(InitialValueSlot);

	for (int i = 0; i < 4; ++i)
		QuickSlotIndex.Add(-1);
}

void AMagicalProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	Controller->SetControlRotation(FRotator(270.f, 270.f, 0.f));

	m_BodyComponent = Cast<USkeletalMeshComponent>(FindComponentByTag(USkeletalMeshComponent::StaticClass(), TEXT("PlayerBody")));

	if (AimWidgetClass != nullptr)
	{
		m_CastingAimWidget = CreateWidget<UUserWidget>(GetWorld(), AimWidgetClass);
		if (m_CastingAimWidget != nullptr)
			m_CastingAimWidget->AddToViewport();
	}

	if (InventoryWidgetClass != nullptr)
	{
		m_InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
		if (m_InventoryWidget != nullptr)
			m_InventoryWidget->AddToViewport(1);
	}

	m_aActiveWeapon = nullptr;

	m_vCameraOffset = FVector(0.f, 70.f, 45.f);
	CameraBoom->SocketOffset = m_vCameraOffset;
	m_vCameraOffset = CameraBoom->SocketOffset;
	m_fCameraDistance = CameraBoom->TargetArmLength;

	m_fMaxHP = HP;
	m_fMaxMP = MP;
	m_fMaxSP = Stamina;

	m_CastingAimWidget->SetVisibility(ESlateVisibility::Hidden);
	m_InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	WakeUpAnimPlay = true;

	m_Controller = Cast<ACustomPlayerController>(GetController());
}

void AMagicalProjectCharacter::Tick(float DeltaTime)	
{
	Super::Tick(DeltaTime);	

	if (m_aLineHitActor)
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, m_aLineHitActor->GetActorLabel());

	//for (int i = 0; i < QuickSlotIndex.Num(); i++)
	//{
	//	FString Message2 = FString::Printf(TEXT("%d"), QuickSlotIndex[i]);
	//	GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, Message2);
	//}

	if (WakeUpAnimPlay)
		return;

	switch (m_ePlayerStatus)
	{
	case EPlayerStatus::IDLE:
		IdleFunction(DeltaTime);
		break;
	case EPlayerStatus::DASH:
		DashFunction(DeltaTime);
		break;
	case EPlayerStatus::RUN:
		RunFunction();
		break;
	case EPlayerStatus::CASTING:
		CastFunction();
		break;
	case EPlayerStatus::PUNCHATTACK:
		PunchAttackFunction(DeltaTime);
		break;
	case EPlayerStatus::MAGICATTACK:
		CastFunction();
		MagicAttackFunction(DeltaTime);
		break;
	case EPlayerStatus::WEAPONATTACK:
		WeaponAttackFunction(DeltaTime);
		break;
	default:
		break;
	}

	DefaultFunction(DeltaTime);	
}

void AMagicalProjectCharacter::FireProjectile(int _iCastNum)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Fire!"));

	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		if (_iCastNum == 1 && MagicAttack01)
			World->SpawnActor<APlayerProjectile>(MagicAttack01, m_vRightHandLocation, m_vCameraRotation, ActorSpawnParams);
		else if (_iCastNum == 2 && MagicAttack02)
			World->SpawnActor<APlayerProjectile>(MagicAttack02, m_vCameraLocation, m_vCameraRotation, ActorSpawnParams);
		else if (_iCastNum == 3 && MagicAttack03)
			World->SpawnActor<APlayerProjectile>(MagicAttack03, m_vCameraLocation, m_vCameraRotation, ActorSpawnParams);
	}
}

void AMagicalProjectCharacter::DefaultFunction(float _DT)
{
	m_bLClick = false;

	//======================================Character Rotation======================================//
	if (!GetVelocity().IsNearlyZero() && m_ePlayerStatus != EPlayerStatus::DASH ||	
		m_ePlayerStatus == EPlayerStatus::CASTING ||	
		m_ePlayerStatus == EPlayerStatus::MAGICATTACK ||	
		m_ePlayerStatus == EPlayerStatus::PUNCHATTACK)	
	{
		float fInterpDT = 5.f;

		FRotator CameraYRot(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
		FRotator CharacterRot = FMath::RInterpTo(GetActorRotation(), CameraYRot, _DT, fInterpDT);
		SetActorRotation(CharacterRot);
	}

	if (m_ePlayerStatus == EPlayerStatus::CASTING ||
		m_ePlayerStatus == EPlayerStatus::MAGICATTACK ||
		m_ePlayerStatus == EPlayerStatus::PUNCHATTACK)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = FRotator(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
		float DeltaYaw = FMath::UnwindDegrees(TargetRotation.Yaw - CurrentRotation.Yaw);
		float RotationSpeed = (DeltaYaw / _DT) / 120.f;
		float fTurnSpeed = FMath::Clamp(RotationSpeed, -50.f, 50.f);

		TurnSpeed = FMath::FInterpTo(TurnSpeed, fTurnSpeed, _DT, 10.f);
	}


	//======================================Character MoveVector====================================//
	FVector GlobalVelocity = GetCharacterMovement()->Velocity;	
	FVector LocalVelocity = GetActorTransform().InverseTransformVectorNoScale(GlobalVelocity);

	CharacterMoveVector = FMath::VInterpTo(CharacterMoveVector, LocalVelocity, _DT, 5.f);	

	//======================================Camera Init======================================//
	SettingCamera(_DT, m_fCameraDistance, m_vCameraOffset);

	if (!GetVelocity().IsNearlyZero() && !IsCasting)
	{
		m_fCameraDistance = 250.f;
		m_vCameraOffset = FVector(0.f, 70.f, 45.f);
	}
	else if (IsCasting)
	{
		if (m_bRClick)
		{
			m_fCameraDistance = 100.f;
			m_vCameraOffset = FVector(0.f, 80.f, 80.f);
		}
		else
		{
			m_fCameraDistance = 250.f;
			m_vCameraOffset = FVector(0.f, 70.f, 45.f);
		}
	}

	if (IsRunning && m_ePlayerStatus == EPlayerStatus::RUN)	
		GetCharacterMovement()->bOrientRotationToMovement = true;
	else
		GetCharacterMovement()->bOrientRotationToMovement = false;

	//======================================Player Status======================================//
	HP = FMath::Clamp(HP, 0, m_fMaxHP);
	MP = FMath::Clamp(MP, 0, m_fMaxMP);
	Stamina = FMath::Clamp(Stamina, 0, m_fMaxSP);

	//=======SPRecovery
	if (Stamina == 0.f)
		m_bSPRecovery = false;

	if (!m_bSPRecovery)
		m_fSPRecoveryTime += _DT;

	if (m_fSPRecoveryTime >= 2.f)
	{
		m_bSPRecovery = true;
		m_fSPRecoveryTime = 0.f;
	}

	if (IsRunning || IsRunning && GetCharacterMovement()->IsFalling())
		if (m_ePlayerStatus == EPlayerStatus::DASH)
			Stamina -= 0.4f;
		else
			Stamina -= 0.2f;
	else if (!IsRunning && !GetCharacterMovement()->IsFalling() && m_ePlayerStatus != EPlayerStatus::PUNCHATTACK && m_bSPRecovery)
		Stamina += 0.1f;

	//=======MPRecovery
	if (MP != m_fMaxMP)
		m_fMPRecoveryTime -= _DT;
	else if (MP >= m_fMaxMP)
		m_fMPRecoveryTime = 5.f;

	if (!m_bMPRecovery)
	{
		m_fMPRecoveryTime = 5.f;
		m_bMPRecovery = true;
	}

	if (m_fMPRecoveryTime <= 0.f)
		MP += 0.05f;

	//======================================RunningState=======================================//
	if (GetVelocity().Size2D() > 301.f)
		IsRunning = true;
	else
		IsRunning = false;

	if (m_vMoveVector.Y != 1 && m_ePlayerStatus == EPlayerStatus::RUN ||
		Stamina <= 0.f && m_ePlayerStatus == EPlayerStatus::RUN)
		m_ePlayerStatus = EPlayerStatus::IDLE;

	//====================================AimWidget

	GetActorEyesViewPoint(m_vCameraLocation, m_vCameraRotation);
	m_vRightHandLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));

	if (m_ePlayerStatus == EPlayerStatus::CASTING && MoveEnable||
		m_ePlayerStatus == EPlayerStatus::MAGICATTACK && MoveEnable)
	{
		if (m_vAimWidgetPos != FVector2D(0.f, 0.f))
			m_CastingAimWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		else
			m_CastingAimWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
		m_CastingAimWidget->SetVisibility(ESlateVisibility::Hidden);

	m_CastingAimWidget->SetPositionInViewport(m_vAimWidgetPos);

	//====================================Inventory Widget
	if (m_bIsVisibleInventory)
		m_InventoryWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else
		m_InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

	//======================================Attach Object Function
	if (m_aActiveWeapon)
	{
		IsItemEquipped = true;

		ItemSlot[iWeaponEquipSlotNum].MaxDurability = m_aActiveWeapon->GetMaxDurability();
		ItemSlot[iWeaponEquipSlotNum].Durability = m_aActiveWeapon->GetDurability();

		if (m_aActiveWeapon->IsDurabilityIsZero())
		{
			m_aActiveWeapon = nullptr;
			//ItemSlot[iWeaponEquipSlotNum] = InitialValueSlot;
			DeleteSlot(iWeaponEquipSlotNum);
			iWeaponEquipSlotNum = -1;

			ToolTipWidgetVisible = true;
			ToolTipWidgetText = TEXT("무기의 내구도가 소진되어 파괴되었다!");

			OnInventoryUpdated.Broadcast();
		}
	}
	else
		IsItemEquipped = false;

	if (!IsCasting && m_aCastingLight)
	{
		m_aCastingLight->Destroy();
		m_aCastingLight = nullptr;
	}

	//=========================================ToolTipWidgetVisible
	if (ToolTipWidgetVisible)
	{
		m_fTTViewCoolTime += _DT;

		if (m_fTTViewCoolTime >= 5.f)
		{
			m_fTTViewCoolTime = 0.f;
			ToolTipWidgetVisible = false;
			ToolTipWidgetText = TEXT(" ");
		}
	}

	//=========================================Line Trace with Object
	ViewportTransformData();
	FindObjByLineTrace();
}

void AMagicalProjectCharacter::IdleFunction(float _DT)
{
	IsPunchIdle = false;
	IsCasting = false;
	IsWeaponAttack = false;
	m_fAttackTime = 0.f;
	m_fMeleeComboTime = 0.f;
	AttackComboNum = 0;

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;

	if(!IsCrouching)
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AMagicalProjectCharacter::RunFunction()
{
	IsCasting = false;
	IsCrouching = false;
	IsPunchIdle = false;
	IsWeaponAttack = false;

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->JumpZVelocity = 900.f;
	GetCharacterMovement()->AirControl = 0.7f;
}

void AMagicalProjectCharacter::CastFunction()	
{
	IsCasting = true;
	IsWeaponAttack = false;

	//==================== Aim Function
	UWorld* const World = GetWorld();	
	
	FHitResult HitResult;
	//FVector Start = m_vCameraLocation;
	FVector Start = m_vRightHandLocation;
	FVector End = Start + (m_vCameraRotation.Vector() * 10000);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);	

	bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, CollisionParams);
	
	if (bHit)
	{
		FVector2D ScreenPosition;	
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);	
		PlayerController->ProjectWorldLocationToScreen(HitResult.Location, ScreenPosition);	
		m_vAimWidgetPos = ScreenPosition;
	}
	else
		m_vAimWidgetPos = FVector2D(0.f, 0.f);

	if (!m_aCastingLight)
	{
		FActorSpawnParameters ActorSpawnParams;	
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;	
		m_aCastingLight = GetWorld()->SpawnActor<AWeapons_Active>(Casting_Light, m_vRightHandLocation, m_vCameraRotation, ActorSpawnParams);
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);	
		m_aCastingLight->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("RightHand"));
	}
}

void AMagicalProjectCharacter::DashFunction(float _DT)
{
	if (m_fDashTime == 0.f)
		IsDash = true;

	if (IsDash)
		m_fDashTime += _DT;

	if (m_fDashTime >= 0.8f)
	{
		IsDash = false;
		m_fDashTime = 0.f;
		m_ePlayerStatus = m_ePrevStatus;
	}
}

void AMagicalProjectCharacter::MagicAttackFunction(float _DT)
{
	IsAttack = true;
	IsWeaponAttack = false;

	if(m_fAttackTime == 0.f)
		m_bMPRecovery = false;

	if (m_ePrevStatus == EPlayerStatus::CASTING)
	{
		if (CastNum == 1 && m_fAttackTime == 0.f)
		{
			MP -= 40;
			m_fAttackTime = 2.2f;
		}
		else if (CastNum == 2 && m_fAttackTime == 0.f)
		{
			MP -= 60;
			m_fAttackTime = 1.9f;
		}
		else if (CastNum == 9 && m_fAttackTime == 0.f)
		{
			MP -= 80;
			m_fAttackTime = 3.f;
		}
	}

	m_fAttackTime -= _DT;

	if (m_fAttackTime <= 0.f)
	{
		IsAttack = false;
		CastNum = 0;
		m_fAttackTime = 0.f;
		m_ePlayerStatus = m_ePrevStatus;
	}
}

void AMagicalProjectCharacter::PunchAttackFunction(float _DT)
{
	IsPunchIdle = true;
	IsAttack = true;
	IsWeaponAttack = false;
	IsCasting = false;

	if (m_ePrevStatus == EPlayerStatus::IDLE)
	{
		if (m_bLClick && m_fAttackTime <= 0.f)
		{
			if (AttackComboNum == 1)
				m_fAttackTime = 0.6f;
			else if (AttackComboNum == 2)
				m_fAttackTime = 0.8f;
			else if (AttackComboNum == 3)
				m_fAttackTime = 1.1f;
		
			Stamina -= 2.f;
			m_fMeleeComboTime = 0.f;
			m_bPunchIdleWait = false;
		}
	}

	m_fAttackTime -= _DT;

	if (m_fAttackTime <= 0.f)
	{
		IsAttack = false;
		m_fAttackTime = 0.f;

		if (!m_bPunchIdleWait)
		{
			m_bPunchIdleWait = true;
			m_fMeleeComboTime = 1.f;
		}

		m_fMeleeComboTime -= _DT;
	}

	if (m_fMeleeComboTime <= 0.f && m_fAttackTime <= 0.f)
	{
		m_fMeleeComboTime = 0.f;
		AttackComboNum = 0;
		IsAttack = false;
		m_bPunchIdleWait = false;
		m_ePlayerStatus = m_ePrevStatus;
	}
}

void AMagicalProjectCharacter::WeaponAttackFunction(float _DT)
{
	IsPunchIdle = false;
	IsCasting = false;
	IsWeaponAttack = true;

	if (!IsItemEquipped)
		m_ePlayerStatus = EPlayerStatus::IDLE;
}

//================================================ Input

void AMagicalProjectCharacter::SetMoveEnable(UAnimMontage* Montage, bool bInterrupted)
{
	MoveEnable = true;
}

void AMagicalProjectCharacter::WakeUpAnimEnd()
{
	WakeUpAnimPlay = false;
}

void AMagicalProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputSpace);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMagicalProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMagicalProjectCharacter::Look);

		//============================================Custom Action============================================//
		EnhancedInputComponent->BindAction(MouseWheel, ETriggerEvent::Triggered, this, &AMagicalProjectCharacter::SetCameraDist);
		EnhancedInputComponent->BindAction(Keyboard_Arrow, ETriggerEvent::Triggered, this, &AMagicalProjectCharacter::SetCameraOffset);
		EnhancedInputComponent->BindAction(LCtrl, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputLCtrl);
		EnhancedInputComponent->BindAction(LCtrl, ETriggerEvent::Completed, this, &AMagicalProjectCharacter::InputLCtrl_End);
		EnhancedInputComponent->BindAction(LShift, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputLShift);
		EnhancedInputComponent->BindAction(LShift, ETriggerEvent::Completed, this, &AMagicalProjectCharacter::InputLShift_End);
		EnhancedInputComponent->BindAction(Keyboard_C, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyC);
		EnhancedInputComponent->BindAction(Keyboard_E, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyE);
		EnhancedInputComponent->BindAction(Keyboard_R, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyR);
		EnhancedInputComponent->BindAction(Keyboard_Q, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyQ);
		EnhancedInputComponent->BindAction(Num_4, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputNum_4);
		EnhancedInputComponent->BindAction(LClick, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputLClick);
		EnhancedInputComponent->BindAction(RClick, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputRClick);
		EnhancedInputComponent->BindAction(RClick, ETriggerEvent::Completed, this, &AMagicalProjectCharacter::InputRClick_End);	
		EnhancedInputComponent->BindAction(Num_1, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputNum_1);
		EnhancedInputComponent->BindAction(Num_2, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputNum_2);
		EnhancedInputComponent->BindAction(Num_3, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputNum_3);
		EnhancedInputComponent->BindAction(Keyboard_U, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyU);
		EnhancedInputComponent->BindAction(Keyboard_I, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyI);
		EnhancedInputComponent->BindAction(Keyboard_O, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyO);
		EnhancedInputComponent->BindAction(Keyboard_P, ETriggerEvent::Started, this, &AMagicalProjectCharacter::InputKeyP);
	}
	else
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
}

void AMagicalProjectCharacter::Move(const FInputActionValue& Value)
{
	if (WakeUpAnimPlay)
	{
		if (!m_bIsTutorialMontagePlaying)
		{
			m_bIsTutorialMontagePlaying = true;

			UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
			AnimInst->Montage_Play(FullMontage, 1.f);
			AnimInst->Montage_JumpToSection(FName("StandingUp"), FullMontage);


		}
		return;
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	m_vMoveVector = Value.Get<FVector2D>();

	if (Controller != nullptr && MoveEnable)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMagicalProjectCharacter::Look(const FInputActionValue& Value)
{
	//if (WakeUpAnimPlay)
	//	return;

	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);	
	}
}

void AMagicalProjectCharacter::InputSpace()
{
	if (WakeUpAnimPlay)
		return;
	
	Jump();
}

void AMagicalProjectCharacter::InputLCtrl()
{
	if (WakeUpAnimPlay)
		return;

	if(!IsRunning && !GetCharacterMovement()->IsFalling() &&
		m_ePlayerStatus != EPlayerStatus::DASH)
	{
		IsCrouching = true;

		GetCharacterMovement()->MaxWalkSpeed = 150.f;
	}
}

void AMagicalProjectCharacter::InputLCtrl_End()
{
	if (WakeUpAnimPlay)
		return;

	if (IsCrouching)
	{
		IsCrouching = false;

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}



void AMagicalProjectCharacter::InputLShift()
{
	if (WakeUpAnimPlay)
		return;

	if (m_vMoveVector.Y == 1 && !IsCrouching && Stamina > 0.f &&
		m_ePlayerStatus != EPlayerStatus::MAGICATTACK &&
		m_ePlayerStatus != EPlayerStatus::WEAPONATTACK &&
		m_ePlayerStatus != EPlayerStatus::PUNCHATTACK &&	
		m_ePlayerStatus != EPlayerStatus::DASH)
		m_ePlayerStatus = EPlayerStatus::RUN;
	else if (m_ePlayerStatus == EPlayerStatus::RUN ||	
		m_ePlayerStatus == EPlayerStatus::CASTING ||
		IsCrouching)
		m_ePlayerStatus = EPlayerStatus::IDLE;
}

void AMagicalProjectCharacter::InputLShift_End()
{
	if (WakeUpAnimPlay)
		return;

	if (m_ePlayerStatus == EPlayerStatus::RUN)	
		m_ePlayerStatus = EPlayerStatus::IDLE;	
}

void AMagicalProjectCharacter::InputKeyC()
{
	if (WakeUpAnimPlay)
		return;

	if (Stamina > 15.f &&	
		!GetVelocity().IsNearlyZero() &&	
		!GetCharacterMovement()->IsFalling() &&
		m_ePlayerStatus != EPlayerStatus::MAGICATTACK &&
		m_ePlayerStatus != EPlayerStatus::DASH)	
	{
		m_ePrevStatus = m_ePlayerStatus;
	
		m_ePlayerStatus = EPlayerStatus::DASH;
	
		FVector XYVector = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.f);
		LaunchCharacter(XYVector.GetSafeNormal() * 2000.f, true, false);
	}	
}

void AMagicalProjectCharacter::InputKeyR()
{
	if (WakeUpAnimPlay)
		return;

	if (m_ePlayerStatus == EPlayerStatus::CASTING &&
		MP > m_fCastingNeedMP2)
	{
		m_ePrevStatus = EPlayerStatus::CASTING;
		m_ePlayerStatus = EPlayerStatus::MAGICATTACK;
		CastNum = 2;
	}
}

void AMagicalProjectCharacter::InputKeyE()
{
	if (WakeUpAnimPlay)
		return;

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	AInteractiveObj* IAObj = Cast<AInteractiveObj>(m_aLineHitActor);

	if (IAObj)
	{
		if (IAObj->IsLocked)
		{
			for (auto Item : ItemSlot)
			{
				if (Item.ItemName == IAObj->NeedKeyName)
				{
					IAObj->OpenLockedDoor();

					// ToolTip Widget Visible & Set Text
					if (m_fTTViewCoolTime > 0.f)
						m_fTTViewCoolTime = 0.f;

					ToolTipWidgetVisible = true;
					ToolTipWidgetText = TEXT("열쇠로 잠긴 문을 열었다.");

					break;
				}
			}
			if (IAObj->IsLocked)
			{
				IAObj->LockedSoundPlay();

				// ToolTip Widget Visible & Set Text 
				if (m_fTTViewCoolTime > 0.f)
					m_fTTViewCoolTime = 0.f;

				ToolTipWidgetVisible = true;
				ToolTipWidgetText = TEXT("잠겨있다.\n열쇠가 필요할 듯 하다.");

			}
		}
		else if (!IAObj->IsLocked)
			IAObj->ActiveObject();
	}

	//=========================Item PickUP=========================//

	AItems* Items = Cast<AItems>(m_aLineHitActor);
	
	if (Items)
	{
		AddItemSlot(Items->GetItemData());
		Items->Destroy();	
	}
}

void AMagicalProjectCharacter::InputKeyQ()	
{
	if (WakeUpAnimPlay)
		return;

	if (m_ePlayerStatus == EPlayerStatus::CASTING &&
		!GetCharacterMovement()->IsFalling() && 
		MP > m_fCastingNeedMP9)
	{
		m_ePrevStatus = EPlayerStatus::CASTING;
		m_ePlayerStatus = EPlayerStatus::MAGICATTACK;
		CastNum = 9;

		IsCrouching = false;
		MoveEnable = false;
		
		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(ShieldCasting, 1.f);
		AnimInst->Montage_JumpToSection(FName("ShieldCast"), ShieldCasting);
		FOnMontageEnded Delegate;	
		Delegate.BindUObject(this, &AMagicalProjectCharacter::SetMoveEnable);
		AnimInst->Montage_SetEndDelegate(Delegate, ShieldCasting);
	}
}

void AMagicalProjectCharacter::InputNum_4()
{
	if (WakeUpAnimPlay)
		return;

	if (!m_bIsVisibleInventory)
		m_bIsVisibleInventory = true;
	else
		m_bIsVisibleInventory = false;

	if(m_Controller)
		m_Controller->ShowCursor(m_bIsVisibleInventory);
}

void AMagicalProjectCharacter::InputKeyU()
{
	UseQuickSlot(0);
}

void AMagicalProjectCharacter::InputKeyI()
{
	UseQuickSlot(1);
}

void AMagicalProjectCharacter::InputKeyO()
{
	UseQuickSlot(2);
}

void AMagicalProjectCharacter::InputKeyP()
{
	UseQuickSlot(3);
}

void AMagicalProjectCharacter::InputLClick()
{
	if (WakeUpAnimPlay)
		return;

	if (m_ePlayerStatus != EPlayerStatus::RUN)
	{
		m_bLClick = true;

		if (m_ePlayerStatus == EPlayerStatus::CASTING &&								// CastingAttack
			MP > m_fCastingNeedMP1)
		{
			m_ePrevStatus = EPlayerStatus::CASTING;	
			m_ePlayerStatus = EPlayerStatus::MAGICATTACK;	
			CastNum = 1;
		}
		else if (m_ePlayerStatus == EPlayerStatus::IDLE && !m_bIsWeaponChange ||		// PunchAttack
			m_ePlayerStatus == EPlayerStatus::PUNCHATTACK && !m_bIsWeaponChange)
		{
			if (Stamina > 2.f)
			{
				m_ePrevStatus = EPlayerStatus::IDLE;
				m_ePlayerStatus = EPlayerStatus::PUNCHATTACK;

				if (m_fAttackTime <= 0.f)
					AttackComboNum++;

				if (AttackComboNum > 3)
					AttackComboNum = 1;
			}
		}
		else if (m_ePlayerStatus == EPlayerStatus::WEAPONATTACK && !m_bIsWeaponChange)	// WeaponAttack
		{
			if (Stamina > 5.f)
			{
				UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();	
				FString MontageName;
				if (!AnimInst->IsAnyMontagePlaying())
				{
					AttackComboNum++;

					if (AttackComboNum > 2)
						AttackComboNum = 1;

					if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")) ||
						m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))
						MontageName = FString::Printf(TEXT("Attack_%d"), AttackComboNum);
					else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
					{
						if (AttackComboNum == 1)
							MontageName = FString::Printf(TEXT("KnifeAttack_%d"), AttackComboNum);
						else
							MontageName = TEXT("Attack_1");
					}

					AnimInst->Montage_Play(WeaponAttack, 1.3f);
					AnimInst->Montage_JumpToSection(FName(MontageName), WeaponAttack);
				}
			}
		}
	}
}

void AMagicalProjectCharacter::InputRClick()
{
	if (WakeUpAnimPlay)
		return;

	m_bRClick = true;
}

void AMagicalProjectCharacter::InputRClick_End()
{
	if (WakeUpAnimPlay)
		return;

	m_bRClick = false;
}

void AMagicalProjectCharacter::InputNum_1()
{
	if (WakeUpAnimPlay)
		return;

	if (m_ePlayerStatus != EPlayerStatus::DASH &&
		m_ePlayerStatus != EPlayerStatus::RUN &&
		m_ePlayerStatus != EPlayerStatus::PUNCHATTACK &&
		m_ePlayerStatus != EPlayerStatus::WEAPONATTACK
		)
	{
		m_ePlayerStatus = EPlayerStatus::CASTING;

		if(FingersnapSound)
			UGameplayStatics::PlaySoundAtLocation(this, FingersnapSound, GetActorLocation());

		if (BurnerSound)
			UGameplayStatics::PlaySoundAtLocation(this, BurnerSound, GetActorLocation());
	}
}

void AMagicalProjectCharacter::InputNum_2()
{
	if (WakeUpAnimPlay)
		return;

	if (IsItemEquipped &&
		m_ePlayerStatus != EPlayerStatus::RUN &&
		m_ePlayerStatus != EPlayerStatus::CASTING &&
		m_ePlayerStatus != EPlayerStatus::MAGICATTACK &&
		m_ePlayerStatus != EPlayerStatus::PUNCHATTACK &&
		m_ePlayerStatus != EPlayerStatus::WEAPONATTACK)
	{
		m_bIsWeaponChange = true;
		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();	
		AnimInst->Montage_Play(WeaponDrawSheath, 1.f);

		if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")) ||	
			m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))	
			AnimInst->Montage_JumpToSection(FName("SMWeaponDraw"), WeaponDrawSheath);
		else if(m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
			AnimInst->Montage_JumpToSection(FName("KnWeaponDraw"), WeaponDrawSheath);
	}	
}

void AMagicalProjectCharacter::InputNum_3()
{
	if (WakeUpAnimPlay)
		return;

	if (!IsAttack)
	{
		m_ePlayerStatus = EPlayerStatus::IDLE;
		m_fCameraDistance = 250.f;
		m_vCameraOffset = FVector(0.f, 70.f, 45.f);


		if (IsItemEquipped && m_ePrevStatus == EPlayerStatus::WEAPONATTACK)
		{
			m_bIsWeaponChange = true;
			m_ePrevStatus = EPlayerStatus::IDLE;
			UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();	
			AnimInst->Montage_Play(WeaponDrawSheath, 1.f);

			if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")) ||
				m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))
				AnimInst->Montage_JumpToSection(FName("SMWeaponSheath"), WeaponDrawSheath);
			else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
				AnimInst->Montage_JumpToSection(FName("KnWeaponSheath"), WeaponDrawSheath);
		}
	}
}

void AMagicalProjectCharacter::SetCameraDist(const FInputActionValue& Value)
{
	if (WakeUpAnimPlay)
		return;

	if (WakeUpAnimPlay)
		return;

	m_fCameraDistance += (Value.Get<float>()) * 15.f;
	m_fCameraDistance = FMath::Clamp(m_fCameraDistance, -50.f, 400.f);

	CameraBoom->TargetArmLength = m_fCameraDistance;
}

void AMagicalProjectCharacter::SetCameraOffset(const FInputActionValue& Value)
{
	if (WakeUpAnimPlay)
		return;

	FVector2D SetOffset = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		m_vCameraOffset.Z += SetOffset.Y;
		m_vCameraOffset.Y += SetOffset.X;
		m_vCameraOffset.Z = FMath::Clamp(m_vCameraOffset.Z, -75.f, 75.f);
		m_vCameraOffset.Y = FMath::Clamp(m_vCameraOffset.Y, -75.f, 75.f);

		CameraBoom->SocketOffset = m_vCameraOffset;
	} 
}

void AMagicalProjectCharacter::SettingCamera(float _DT, float _Length, FVector _Offset)
{
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, _Length, _DT, 3.f);
	CameraBoom->SocketOffset.Y = FMath::FInterpTo(CameraBoom->SocketOffset.Y, _Offset.Y, _DT, 3.f);
	CameraBoom->SocketOffset.Z = FMath::FInterpTo(CameraBoom->SocketOffset.Z, _Offset.Z, _DT, 3.f);
}

void AMagicalProjectCharacter::ChangeItemSlot(int32 _fromIdx, int32 _toIdx)
{
	if (_fromIdx >= ItemSlot.Num() || _toIdx >= ItemSlot.Num())
		return;

	if (_fromIdx == _toIdx)
		return;

	if (ItemSlot[_fromIdx].IsEquipped ||
		ItemSlot[_toIdx].IsEquipped)
		return;

	FInvenSlot TempSlot;

	if (ItemSlot[_fromIdx].ItemName == ItemSlot[_toIdx].ItemName)
	{
		if (ItemSlot[_fromIdx].Amount != ItemSlot[_fromIdx].MaxAmount &&
			ItemSlot[_toIdx].Amount != ItemSlot[_toIdx].MaxAmount)
		{
			int32 iTotalAmount = ItemSlot[_fromIdx].Amount + ItemSlot[_toIdx].Amount;

			if (iTotalAmount < ItemSlot[_toIdx].MaxAmount)						// a와 b슬롯 아이템의 개수가 해당 아이템의 최대 개수를 넘지 못할 경우
			{
				TempSlot = InitialValueSlot;
				ItemSlot[_toIdx].Amount += ItemSlot[_fromIdx].Amount;
			}
			else if (iTotalAmount > ItemSlot[_toIdx].MaxAmount)					// 최대 개수를 넘었을 경우
			{
				int32 toIdxNeedAmount = ItemSlot[_toIdx].MaxAmount - ItemSlot[_toIdx].Amount;

				TempSlot.ItemName = ItemSlot[_fromIdx].ItemName;
				TempSlot.Amount = ItemSlot[_fromIdx].Amount - toIdxNeedAmount;
				TempSlot.MaxAmount = ItemSlot[_fromIdx].MaxAmount;
				TempSlot.ItemType = ItemSlot[_fromIdx].ItemType;

				ItemSlot[_toIdx].Amount = ItemSlot[_toIdx].MaxAmount;
			}
			else																// 합이 딱 최대 개수일 경우
			{
				TempSlot = InitialValueSlot;
				ItemSlot[_toIdx].Amount = ItemSlot[_toIdx].MaxAmount;
			}
			ItemSlot[_fromIdx] = TempSlot;
		}
		else																	// 두 슬롯 중 하나라도 최대 개수일 경우
		{
			TempSlot = ItemSlot[_fromIdx];	
			ItemSlot[_fromIdx] = ItemSlot[_toIdx];	
			ItemSlot[_toIdx] = TempSlot;	

			// Update QuickSlot
			for (int i = 0; i < QuickSlotIndex.Num(); i++)
				if (QuickSlotIndex[i] == _fromIdx || QuickSlotIndex[i] == _toIdx)
				{
					if (QuickSlotIndex[i] == _fromIdx)
						QuickSlotIndex[i] = _toIdx;
					else
						QuickSlotIndex[i] = _fromIdx;
				}

		}
	}
	else																		// 두 슬롯의 아이템 이름이 다를 경우
	{
		TempSlot = ItemSlot[_fromIdx];	
		ItemSlot[_fromIdx] = ItemSlot[_toIdx];
		ItemSlot[_toIdx] = TempSlot;

		// Update QuickSlot
		for (int i = 0; i < QuickSlotIndex.Num(); i++)
			if (QuickSlotIndex[i] == _fromIdx || QuickSlotIndex[i] == _toIdx)
			{
				if (QuickSlotIndex[i] == _fromIdx)
					QuickSlotIndex[i] = _toIdx;
				else
					QuickSlotIndex[i] = _fromIdx;
			}

	}

	OnInventoryUpdated.Broadcast();
}

void AMagicalProjectCharacter::UseItemSlot(int32 _SlotIdx)
{
	switch (ItemSlot[_SlotIdx].ItemType)
	{
	case EItemType::CONSUMABLE:
		if (ItemSlot[_SlotIdx].ItemName == FName(TEXT("Donut")))
			HP += 10.f;
		else if (ItemSlot[_SlotIdx].ItemName == FName(TEXT("Soda")))
			MP += 10.f;
		else if (ItemSlot[_SlotIdx].ItemName == FName(TEXT("EnergyDrink")))
			Stamina += 20.f;
		 
		//...

		ItemSlot[_SlotIdx].Amount -= 1;
		break;
	case EItemType::WEAPON:

		if (ItemSlot[_SlotIdx].IsEquipped == false)
		{
			if (iWeaponEquipSlotNum == -1)
			{
				FActorSpawnParameters ActorSpawnParams;	
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;	
				UClass* WeaponClass = LoadWeaponClass(ItemSlot[_SlotIdx].ItemName);	

				if (WeaponClass != NULL)	
				{
					m_aActiveWeapon = GetWorld()->SpawnActor<AWeapons_Active>(WeaponClass, m_vRightHandLocation, m_vCameraRotation, ActorSpawnParams);	
					const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
					m_aActiveWeapon->SetDurability(ItemSlot[_SlotIdx].Durability);
					m_aActiveWeapon->SetMaxDurability(ItemSlot[_SlotIdx].MaxDurability);

					if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")) ||
						m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))
					{
						m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("WeaponSlot"));
						EquipWeaponNum = 0;
					}
					else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
					{
						m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("KnifeSlot"));
						EquipWeaponNum = 1;
					}

					iWeaponEquipSlotNum = _SlotIdx;	
					ItemSlot[_SlotIdx].IsEquipped = true;	
				}
			}
			else
			{
				if (m_ePlayerStatus == EPlayerStatus::WEAPONATTACK)	
					break;	

				if (IsValid(m_aActiveWeapon))
				{
					m_aActiveWeapon->Destroy();
					m_aActiveWeapon = nullptr;
					ItemSlot[iWeaponEquipSlotNum].IsEquipped = false;
				}
				FActorSpawnParameters ActorSpawnParams;	
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;	
				UClass* WeaponClass = LoadWeaponClass(ItemSlot[_SlotIdx].ItemName);	
				if (WeaponClass != NULL)	
				{
					m_aActiveWeapon = GetWorld()->SpawnActor<AWeapons_Active>(WeaponClass, m_vRightHandLocation, m_vCameraRotation, ActorSpawnParams);	
					const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
					m_aActiveWeapon->SetDurability(ItemSlot[_SlotIdx].Durability);
					m_aActiveWeapon->SetMaxDurability(ItemSlot[_SlotIdx].MaxDurability);

					if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")) ||
						m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))
					{
						m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("WeaponSlot"));
						EquipWeaponNum = 0;
					}
					else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
					{
						m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("KnifeSlot"));
						EquipWeaponNum = 1;
					}

					iWeaponEquipSlotNum = _SlotIdx;	
					ItemSlot[_SlotIdx].IsEquipped = true;	
				}
			}
		}
		else
		{
			if (m_ePlayerStatus == EPlayerStatus::WEAPONATTACK)
				break;

			if (IsValid(m_aActiveWeapon))
			{
				m_aActiveWeapon->Destroy();	
				m_aActiveWeapon = nullptr;	
				ItemSlot[_SlotIdx].IsEquipped = false;	
				iWeaponEquipSlotNum = -1;	
			}
		}
		break;
	case EItemType::DEFAULT:

		break;
	}
	
	// if Item Amount is Zero..
	if (ItemSlot[_SlotIdx].Amount <= 0)
		DeleteSlot(_SlotIdx);

	// Update QuickSlot
	for (int i = 0; i < QuickSlotIndex.Num(); i++)
		if (QuickSlotIndex[i] == _SlotIdx)
			if (ItemSlot[_SlotIdx].Amount <= 0)
				QuickSlotIndex[i] = -1;

	OnInventoryUpdated.Broadcast();
}

void AMagicalProjectCharacter::DeleteSlot(int32 _SlotIdx)
{
	ItemSlot[_SlotIdx] = InitialValueSlot;
}

void AMagicalProjectCharacter::AddItemSlot(FGameItem _Item)
{
	for (int32 i = 0; i < ItemSlot.Num(); ++i)
	{
		if (ItemSlot[i].ItemName == _Item.ItemName)
		{
			if (ItemSlot[i].Amount < _Item.StackSize)
			{
				ItemSlot[i].Amount += 1;
				OnInventoryUpdated.Broadcast();
				return;
			}
		}
		else if (ItemSlot[i].ItemName == NAME_None)
		{
			ItemSlot[i].ItemName = _Item.ItemName;
			ItemSlot[i].Amount = 1;
			ItemSlot[i].MaxAmount = _Item.StackSize;
			ItemSlot[i].ItemType = _Item.ItemType;

			if (ItemSlot[i].ItemType == EItemType::WEAPON)
			{
				ItemSlot[i].MaxDurability = _Item.MaxDurability;
				ItemSlot[i].Durability = _Item.MaxDurability;
			}

			OnInventoryUpdated.Broadcast();
			return;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Full Inventory!"));
}

void AMagicalProjectCharacter::AddQuickSlot(int32 _QuickSlotNum, int32 _SlotIdx)
{
	if (ItemSlot[_SlotIdx].ItemType != EItemType::CONSUMABLE)
		return;

	//// 이미 같은 슬롯이 퀵슬롯에 등록되어 있을 경우 패스
	//for (int i = 0; i < QuickSlotIndex.Num(); i++)
	//	if (QuickSlotIndex[i] == _SlotIdx)
	//		return;

	QuickSlotIndex[_QuickSlotNum] = _SlotIdx;
}

void AMagicalProjectCharacter::UseQuickSlot(int32 _QuickSlotNum)
{
	int SlotNum = QuickSlotIndex[_QuickSlotNum];

	if (SlotNum == -1)
		return;

	UseItemSlot(SlotNum);

	if (ItemSlot[SlotNum].Amount <= 0)
		QuickSlotIndex[_QuickSlotNum] = -1;
}

void AMagicalProjectCharacter::FindObjByLineTrace()
{
	FHitResult HitResult;	
	FVector Start = m_vViewportLocation;
	FVector End = Start + (m_vViewportDirection * 1000);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel2, CollisionParams);

	//FColor LineColor = bHit ? FColor::Green : FColor::Red;	
	//DrawDebugLine(	
	//	GetWorld(),	
	//	Start,
	//	End,
	//	LineColor,
	//	false, // IsInfinityLine
	//	0.5f,  // ViewLineTime
	//	0,     // Depth Test
	//	1.0f   // Line Size
	//);

	if (bHit && (HitResult.ImpactPoint - GetActorLocation()).Size() < 150.f)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor != m_aLineHitActor)
		{
			if (m_aLineHitActor)
			{
				IIAInterface* PreviousIAActor = Cast<IIAInterface>(m_aLineHitActor);
				if (PreviousIAActor)	
					PreviousIAActor->SetWidget(false);	
			}

			m_aLineHitActor = HitActor;
			IIAInterface* IAActor = Cast<IIAInterface>(HitActor);	
			if (IAActor)
				IAActor->SetWidget(true);	
		}
	}
	else
	{
		if (m_aLineHitActor)	
		{
			IIAInterface* IAActor = Cast<IIAInterface>(m_aLineHitActor);	
			if (IAActor)	
				IAActor->SetWidget(false);	

			m_aLineHitActor = nullptr;	
		}
	}
}
//=============================================================AI Stimulus

void AMagicalProjectCharacter::SetupStimulus()
{
	Stimulus->CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	Stimulus->RegisterWithPerceptionSystem();
}

void AMagicalProjectCharacter::ViewportTransformData()
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);	
	FVector2D ScreenCenter = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);	

	FVector WorldLocation;	
	FVector WorldDirection;	
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);	
	UGameplayStatics::DeprojectScreenToWorld(PlayerController, ScreenCenter, OUT WorldLocation, OUT WorldDirection);

	m_vViewportDirection = WorldDirection;
	m_vViewportLocation = WorldLocation;
}

void AMagicalProjectCharacter::SetSheathWeapon()
{
	if (!m_aActiveWeapon)
		return;

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")) ||
		m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))
		m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("WeaponSlot"));
	else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
		m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("KnifeSlot"));

	m_bIsWeaponChange = false;
}

void AMagicalProjectCharacter::SetDrawWeapon()
{
	if (!m_aActiveWeapon)
		return;

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//m_aActiveWeapon->AttachToComponent(GetMesh(), AttachmentRules, TEXT("RightWeapon"));

	if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("WoodStick")))	
		m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("W_WoodStick"));
	else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Hammer")))
		m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("W_Hammer"));
	else if (m_aActiveWeapon->GetActorLabel().Contains(TEXT("Knife")))
		m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("W_Knife"));
	else
		m_aActiveWeapon->AttachToComponent(m_BodyComponent, AttachmentRules, TEXT("W_WoodStick"));


	m_ePlayerStatus = EPlayerStatus::WEAPONATTACK;	
	m_ePrevStatus = EPlayerStatus::WEAPONATTACK;
	m_bIsWeaponChange = false;
}

UClass* AMagicalProjectCharacter::LoadWeaponClass(const FName& _itemName)
{
	if (_itemName == FName(TEXT("WoodStick")))
		return LoadClass<AWeapons_Active>(NULL, TEXT("/Game/CustomAsset/Blueprint/Weapon/WoodStick_Active.WoodStick_Active_C"));	
	else if (_itemName == FName(TEXT("Hammer")))
		return LoadClass<AWeapons_Active>(NULL, TEXT("/Game/CustomAsset/Blueprint/Weapon/Hammer_Active.Hammer_Active_C"));
	else if (_itemName == FName(TEXT("Knife")))
		return LoadClass<AWeapons_Active>(NULL, TEXT("/Game/CustomAsset/Blueprint/Weapon/Knife_Active.Knife_Active_C"));
	//...

	return NULL;
}

void AMagicalProjectCharacter::ActiveWeaponCollision()
{
	if (m_aActiveWeapon)
		m_aActiveWeapon->SetActiveBoxComponent(true);
}

void AMagicalProjectCharacter::DeactiveWeaponCollision()
{
	if (m_aActiveWeapon)
		m_aActiveWeapon->SetActiveBoxComponent(false);
}
