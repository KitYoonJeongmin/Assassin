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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedDelegate);

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
		float CurrentDirection;
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
	FOnFinishedDelegate OnFinished;

	//Montage
public:
	/**몽타주의 우선순위를 계산 후 실행*/
	bool PlayMontagePriority(UAnimMontage* PlayMontage, float PlayRate);

	/**Climbing idle상태의 Montage*/
	void PlayClimbingIdleMon();
	/**Climbing 시작시 올라가기 위한 Montage*/
	void PlayClimbUpMon();

	/**Sword 장착/비장착 Montage*/
	void PlayEquipMontage(bool CanEquip);
	/**Sword 콤보 어택 Montage*/
	void PlaySwordAttackMontage();
	/**Sword에게 맞았을 때*/
	void PlaySwordHitMontage();
	/**Sword Roll*/
	void PlaySwordRollMontage();
	/**Parry Montage*/
	void PlaySwordParryMontage(int32 MontageIndex);
	/**Finisher Montage*/
	class UAnimMontage* PlayFinisherMontage();
	void PlayFinishedMontage(UAnimMontage* FinisherMon);
	void PlaySwordDeathMontage();

	/**Assassination Montage*/
	class UAnimMontage* PlayAssassinMontage();
	void PlayAssassinedMontage(UAnimMontage* FinisherMon);

private:
	//Priority
	TArray<UAnimMontage*> MontagePriority;
	//Climbing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climbing, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* ClimbingIdleMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climbing, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* ClimbUpMon;

	//Sword
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordEquipMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordAttackMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordHitMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordBlockHitMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordRollMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordParryMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwordCombat, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SwordDeathMon;

	//Finisher
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Finisher, Meta = (AllowPrivateAccess = true))
	TMap<UAnimMontage*, UAnimMontage*> SwordFinisherMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Finisher, Meta = (AllowPrivateAccess = true))
	TMap<UAnimMontage*, float> SwordFinisherDistanceMap;

	//Assassination
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Assassination, Meta = (AllowPrivateAccess = true))
	TMap<UAnimMontage*, UAnimMontage*> AssassinationMap;

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
	void MoveToLedge(bool IsStarted);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sword)
	bool IsBlock;

// Finish
	float GetDistancebySwordFinish(UAnimMontage* FinisherMon);
	UFUNCTION()
	void AnimNotify_Death();
	UFUNCTION()
	void AnimNotify_Camera();
	UFUNCTION()
	void AnimNotify_CameraEnd();

private:
//HeadTracking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HeadTracking, Meta = (AllowPrivateAccess = true))
	bool CanHeadTracking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HeadTracking, Meta = (AllowPrivateAccess = true))
	FVector HeadTrackingTargetLoc;	//보간 하는 변수
	FVector HeadTrackingTarget;	//보간 Target 변수
	float HeadTrackingRadius;
	void TrackHead();

};
