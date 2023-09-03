// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Bow.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ABow : public AWeapon
{
	GENERATED_BODY()
	ABow();
public:
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual void Tick(float DeltaSeconds) override;
	virtual void InitializeWeapon(AAssassinCharacter* OwnerCharacter) override;
	UFUNCTION()/**콤보공격 몽타주가 끝났을 때 실행*/
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	USkeletalMeshComponent* GetSkeletalMesh() const { return BowSkeletalMeshComponent; }
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SkeletalMesh")
	USkeletalMeshComponent* BowSkeletalMeshComponent;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UBowAnimInstance* BowAnim;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Arrow")
	class AArrow* CurrentArrow;
	
};
