// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ClimbingComponent.h"
#include "ClimbingMovement.h"
#include "ACAnimInstance.generated.h"

/**
 * 
 */
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
	
	//Montage
public:
	void PlayClimbingIdleMon();
	void PlayClimbUpMon();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climbing, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* ClimbingIdleMon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climbing, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* ClimbUpMon;

	//FootIK
private:
	class UFootIKComponent* FootIKComp;
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
};
