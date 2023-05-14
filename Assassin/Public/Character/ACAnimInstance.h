// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Component/ClimbingComponent.h"
#include "Component/ClimbingMovement.h"
#include "ACAnimInstance.generated.h"

/**
 * 
 */


UENUM(BlueprintType)	
enum class EWeaponState : uint8
{
	E_Sword = 0   UMETA(DisplayName = "Sword"),	
	E_Daggle UMETA(DisplayName = "Daggle"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnableHitCheckDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackDisableHitCheckDelegate);

UCLASS()
class ASSASSIN_API UACAnimInstance : public UAnimInstance, public IClimbingMovement
{
	GENERATED_BODY()
public:
	UACAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = true))
		bool IsFalling;
private:
	UPROPERTY()
	class AAssassinCharacter* Character;

	//Delegate
public:

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackEnableHitCheckDelegate OnEnableAttackHitCheck;
	FOnAttackDisableHitCheckDelegate OnDisableAttackHitCheck;

	//Montage
public:
	/**Climbing idle상태의 Montage*/
	void PlayClimbingIdleMon();
	/**Climbing 시작시 올라가기 위한 Montage*/
	void PlayClimbUpMon();

	/**Sword 장착/비장착 Montage*/
	void PlayEquipMontage(bool CanEquip);
	/**Sword 콤보 어택 Montage*/
	void PlaySwordAttackMontage();
	/**Sword에게 맞았을 때*/
	void PlaySwordHitMontage(FVector AttackVector);
private:
	//Climbing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climbing, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* ClimbingIdleMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climbing, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* ClimbUpMon;

	//Sword
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordAttack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordEquipMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordAttack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordAttackMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordAttack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordHitMon;

	//FootIK
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FRotator FootRotL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FRotator FootRotR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float HipOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float FootOffsetL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float FootOffsetR;

	//HandIK
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK, Meta = (AllowPrivateAccess = true))
	FVector IKLeftHandLoc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HandIK, Meta = (AllowPrivateAccess = true))
	FVector IKRightHandLoc;

	//Climbing
public:
	virtual void UpdateMovementState(EMovementState CurrentState) override;
	void SetCanMoveLedge(bool Left, bool Right);
	UFUNCTION()
	void AnimNotify_ClimbJump();
	UFUNCTION()
	void AnimNotify_ClimbUpEnd();
	UFUNCTION()
	void AnimNotify_MoveToLedge();
	UFUNCTION()
	void AnimNotify_MoveToLedgeEnd();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing)
	EMovementState CurrnetMovementState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing)
	bool CanMoveLeftOnLedge;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing)
	bool CanMoveRightOnLedge;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, Meta = (AllowPrivateAccess = true))
	bool CanMoveOtherLedge;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, Meta = (AllowPrivateAccess = true))
	float InputRightAxis;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, Meta = (AllowPrivateAccess = true))
	float InputForwardAxis;

//Attack
//Sword
public:
	FName GetAttackMontageSectionName(int32 Section);
	void JumpToSwordAttackMontageSection(int32 NewSection);
	UFUNCTION()
	void AnimNotify_CheckNextSection();
	UFUNCTION()
	void AnimNotify_EnableAttackCheck();
	UFUNCTION()
	void AnimNotify_DisableAttackCheck();
	UFUNCTION()
	void AnimNotify_Equipment();
	UFUNCTION()
	void AnimNotify_UnEquipment();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	EWeaponState WeaponState;
};
