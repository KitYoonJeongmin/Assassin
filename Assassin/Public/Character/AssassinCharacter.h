// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Component/ClimbingComponent.h"
#include "Component/ClimbingMovement.h"
#include "Component/EagleVisionInterface.h"
#include "AssassinCharacter.generated.h"


USTRUCT(BlueprintType)
struct FWeapons
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class ASword* SwordWeapon;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class ADagger* DaggleWeapon;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class ABow* BowWeapon;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunEndDelegate);

UCLASS(config=Game)
class AAssassinCharacter : public ACharacter, public IClimbingMovement, public IEagleVisionInterface
{
	GENERATED_BODY()
public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	/** Run Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FallAction;

	/**Attack Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;

	/**Block Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BlockAction;

	/**Equip Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipAction;

	/**Parry Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ParryAction;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	FVector2D MovementVector;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float RunSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float TargetWalkSpeed;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	TArray<USkeletalMeshComponent*> MeshArr;
public:
	AAssassinCharacter();
	
protected:

	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);
	virtual void MoveEnd(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/**JumpStart*/
	void JumpStart();

protected:
	virtual void PostInitializeComponents() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UFUNCTION()
	void RunStart();
	UFUNCTION()
	void Run();
	UFUNCTION()
	void RunEnd();
	UFUNCTION()
	void Fall();
	void SetWalkSpeed(float TargetSpeed);


	//Animation
	public:
	UACAnimInstance* ACAnim;
	UFUNCTION()
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Climbing
	public:
	virtual void UpdateMovementState(EMovementState CurrentState) override;
	EMovementState CurrnetMovementState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing)
	class UClimbingComponent* ClimbingComp;
private:
	bool isWalk = false;

	//FootIK
	public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK)
	class UFootIKComponent* FootIKComp;
	
	//Attack
	public:
	virtual void Attack();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	FOnStunStartDelegate OnStunStart;
	FOnStunEndDelegate OnStunEnd;

	//Block
	public:
	void BlockStart();
	void BlockEnd();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* SparkN;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* BloodN;
	//Parry

	public:
	void TryParry();

	//Weapon
	public:
	void EquipWeapon();
	FWeapons Weapon;
	/**무기를 원하는 소켓에 붙이는 함수*/
	virtual void AttachWeaponTo(class AWeapon* SwitchingWeapon, FName WeaponSocket, bool isEquip);
	AWeapon* GetCurrentWeapon() { return CurrentWeapon; }
protected:
	/**현재 손에 있는 무기*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class AWeapon* CurrentWeapon;

	//Dead
	public:
	bool GetIsDead() { return IsDead; }
	/**죽음 상태로 변경*/
	virtual void Dead();
protected:
	float HealthPoint;
	bool IsDead;

public:
	float GetHealthPoint();

	//Assassination
	public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MotionWarping)
	class UMotionWarpingComponent* MotionWarpingComp;

	//Custom Depth
	public:
	void SetCustomDepth(bool IsEnable);

	//Outline
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Outline)
	class UOutlineComponent* OutlineComponent;
protected:
	//EagleVision
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EagleVision, meta = (AllowPrivateAccess = "true"))
	class UEagleVisionComponent* EagleVisionComponent;
	virtual class UEagleVisionComponent* GetEagleVisionComponent() override;


//Detection
public:
	void EnableDetectionWidget(bool isEnable);
	void UpdateDetectionWidget(float DetectLevel);
protected:
	UPROPERTY(VisibleAnyWhere, Category = UI)
	class UWidgetComponent* DetectionWidget;

	//Detect Enemy
public:

	/**주변에 있는 적들을 찾는 함수*/
	TArray<class AAssassinCharacter*> DetectNearByEnemy(float SearchRadius);
	/**가장 가까운 적을 찾는 함수*/
	class AAssassinCharacter* FindNearestEnemy(TArray<class AAssassinCharacter*> EnemyArr);
protected:
	ETraceTypeQuery DetectEnemyTrace;
};