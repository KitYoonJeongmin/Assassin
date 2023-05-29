// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Daggle.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ADaggle : public AWeapon
{
	GENERATED_BODY()
	

public:
	ADaggle();
	virtual void InitializeWeapon() override;
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION()/**암살 몽타주가 끝났을 때 실행*/
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

//Attack
public:
	virtual void Attack() override;
	void ForwardAttack();
	void UnderAttack();

//Target
public:
	void FindTargetCharacter();
	void FindUnderTargetCharacter();

//Under? Forward
public:
	bool FindCanAttackForward();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Assassination, Meta = (AllowPrivateAccess = true))
	class AAssassinCharacter* TargetCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Assassination, Meta = (AllowPrivateAccess = true))
	bool CanAttackForward;
	
};
