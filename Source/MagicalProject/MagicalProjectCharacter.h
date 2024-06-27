// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons_Active.h"
#include "IAInterface.h"
#include "GameFramework/Character.h"
#include "CustomData.h"
#include "CustomPlayerController.h"
#include "Logging/LogMacros.h"
#include "MagicalProjectCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMagicalProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	//========================================================Custom Action========================================================//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* LCtrl;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* LShift;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* Keyboard_C;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* Keyboard_R;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard_E;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard_Q;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* Keyboard_Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* MouseWheel;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* LClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* RClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* Num_1;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))	
	UInputAction* Num_2;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Num_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Num_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard_U;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard_I;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard_O;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard_P;

public:
	//=============================Player State With Blueprint=============================//
	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	float HP;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	float MP;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	float Stamina;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	FVector CharacterMoveVector;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	float TurnSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	bool IsRunning;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	bool IsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool IsCasting;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool IsPunchIdle;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool IsDash;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool IsAttack;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	int CastNum;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	int AttackComboNum;

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	bool MoveEnable;

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	bool IsWeaponAttack;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	int EquipWeaponNum;

	UPROPERTY(BlueprintReadOnly, Category = "Character")	
	bool IsItemEquipped;

	UPROPERTY(BlueprintReadOnly, Category = "ToolTipWidgetInfo")	
	FString ToolTipWidgetText;	

	UPROPERTY(BlueprintReadOnly, Category = "ToolTipWidgetInfo")
	bool ToolTipWidgetVisible;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool WakeUpAnimPlay;

	//====================================Anim Montage=====================================//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")	
	UAnimMontage* ShieldCasting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	UAnimMontage* WeaponDrawSheath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	UAnimMontage* WeaponAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	UAnimMontage* FullMontage;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetSheathWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetDrawWeapon();

	//=====================================Projectile===================================//
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APlayerProjectile> MagicAttack01;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APlayerProjectile> MagicAttack02;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APlayerProjectile> MagicAttack03;

	//=====================================Anim Notify===================================//
	UFUNCTION(BlueprintCallable, Category = "Function")
	void FireProjectile(int _iCastNum);

	UFUNCTION(BlueprintCallable, Category = "Function")
	void WakeUpAnimEnd();

	//=====================================Attack Notify===================================//
	UFUNCTION(BlueprintCallable, Category = "Function")
	void ActiveWeaponCollision();
	UFUNCTION(BlueprintCallable, Category = "Function")
	void DeactiveWeaponCollision();

	//====================================BluePrintClass====================================//
	UPROPERTY(EditDefaultsOnly, Category = "BluePrintClass")	
	TSubclassOf<class AWeapons_Active> Casting_Light;		

	//====================================WidgetClass====================================//
	UPROPERTY(EditDefaultsOnly, Category = "BluePrintClass")
	TSubclassOf<UUserWidget> AimWidgetClass;	

	UPROPERTY(EditDefaultsOnly, Category = "BluePrintClass")	
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	//====================================ItemSlot====================================//
	UPROPERTY(BlueprintReadOnly, Category = "ItemSlot")
	TArray<FInvenSlot> ItemSlot;

	//==========================QuickSlot==========================//
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<int> QuickSlotIndex;

	//=================================WeaponSlot
	UPROPERTY(BlueprintReadOnly, Category = "ItemSlot")
	int32 iWeaponEquipSlotNum;

	//=================================Inventory Function====================================//
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryUpdateDelegate);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryUpdateDelegate OnInventoryUpdated;

	UFUNCTION(BlueprintCallable, Category = "Function")
	void ChangeItemSlot(int32 _fromIdx, int32 _toIdx);

	UFUNCTION(BlueprintCallable, Category = "Function")
	void UseItemSlot(int32 _SlotIdx);

	UFUNCTION(BlueprintCallable, Category = "Function")
	void DeleteSlot(int32 _SlotIdx);

	UFUNCTION(BlueprintCallable, Category = "Function")	
	void AddQuickSlot(int32 _QuickSlotNum, int32 _SlotIdx);

public:
	AMagicalProjectCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void InputSpace();
	//=================================================Custom Input=================================================//
	void InputLCtrl();
	void InputLCtrl_End();	
	void InputLShift();
	void InputLShift_End();
	void InputKeyC();
	void InputKeyR();	
	void InputKeyE();
	void InputKeyQ();
	void InputLClick();	
	void InputRClick();
	void InputRClick_End();
	void InputNum_1();
	void InputNum_2();
	void InputNum_3();
	void InputNum_4();
	void InputKeyU();
	void InputKeyI();
	void InputKeyO();
	void InputKeyP();
	void SetCameraDist(const FInputActionValue& Value);	
	void SetCameraOffset(const FInputActionValue& Value);
	//=================================================Custom Func=================================================//
	void DefaultFunction(float _DT);
	void IdleFunction(float _DT);
	void RunFunction();
	void CastFunction();
	void DashFunction(float _DT);
	void MagicAttackFunction(float _DT);
	void PunchAttackFunction(float _DT);
	void WeaponAttackFunction(float _DT);
	void SettingCamera(float _DT, float _Length, FVector _Offset);
	void AddItemSlot(FGameItem _Item);
	void FindObjByLineTrace();
	void UseQuickSlot(int32 _QuickSlotNum);

	//=================================================Custom Data=================================================//
	enum class EPlayerStatus	
	{
		IDLE,
		DASH,
		RUN,
		CASTING,
		PUNCHATTACK,
		MAGICATTACK,
		WEAPONATTACK,
	};

	EPlayerStatus			m_ePlayerStatus;
	EPlayerStatus			m_ePrevStatus;

	FVector					m_vCameraOffset;
	FVector2D				m_vMoveVector;
	FVector2D				m_vAimWidgetPos;
	FVector					m_vCameraLocation;
	FVector					m_vRightHandLocation;
	FRotator				m_vCameraRotation;

	float					m_fCameraDistance;
	float					m_fMaxHP;
	float					m_fMaxMP;
	float					m_fMaxSP;

	int32					m_AmountLeft;

	//==================================================CoolTime
	float					m_fSPRecoveryTime;
	float					m_fDashTime;
	float					m_fAttackTime;
	float					m_fMeleeComboTime;
	float					m_fMPRecoveryTime;
	float					m_fTTViewCoolTime;

	float					m_fCastingNeedMP1;
	float					m_fCastingNeedMP2;
	float					m_fCastingNeedMP9;

	bool					m_bSPRecovery;
	bool					m_bMPRecovery;
	bool					m_bLClick;
	bool					m_bPunchIdleWait;
	bool					m_bRClick;
	bool					m_bIsWeaponChange;
	bool					m_bIsVisibleInventory;
	bool					m_bIsTutorialMontagePlaying;

	//============================Weapon
	AWeapons_Active*		m_aActiveWeapon;
	AWeapons_Active*		m_aCastingLight;

	//============================Widget
	UUserWidget* m_CastingAimWidget;
	UUserWidget* m_InventoryWidget;
	//UUserWidget* m_QuickSlotWidget;

	//============================Controller
	ACustomPlayerController* m_Controller;

	//=====================Anim Delegete=================//
	void SetMoveEnable(UAnimMontage* Montage, bool bInterrupted);

	//=====================AI Stimulus=====================//
	class UAIPerceptionStimuliSourceComponent* Stimulus;
	void SetupStimulus();

	//=========================LineTraceObject=========================//
	void ViewportTransformData();
	FVector				m_vViewportDirection;
	FVector				m_vViewportLocation;
	AActor*				m_aLineHitActor;


	FInvenSlot InitialValueSlot;


	//==========================Load WeaponClass
	UClass* LoadWeaponClass(const FName& _itemName);

	//==========================Player Body Component
	USkeletalMeshComponent* m_BodyComponent;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

