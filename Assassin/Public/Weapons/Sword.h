// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ASword : public AWeapon
{
	GENERATED_BODY()
public:
	ASword();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeWeapon(AAssassinCharacter* OwnerCharacter) override;

	//Attack
public:
	UFUNCTION()/**콤보공격 몽타주가 끝났을 때 실행*/
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	/**콤보 공격이 시작될 때 State를 설정*/
	void AttackStartComboState();
	/**콤보 공격이 끝날 때 State를 설정*/
	void AttackEndComboState();
	/**공격을 체크하기위해 LineTrace를 공격범위에 생성*/
	void AttackCheck();
	/**가장 가까운 적앞으로 이동*/
	void MoveToNearestEnemy();
	/**콤보 공격을 실행하는 함수*/
	virtual void Attack();

	UFUNCTION()
	void OnEnableAttackCheck();
	UFUNCTION()
	void OnDisableAttackCheck();
	UFUNCTION()
	void OnNextAttackCheck();

public:
	ETraceTypeQuery SwordAttackType;
protected:
	bool CanNextCombo;
	bool CanAttackCheck;
	bool IsComboInputOn;
	int32 CurrentCombo;
	FVector AttackHitVec;
	TArray<AActor*> ActorsToIgnore;

	float EnemyDetectRange;
	float EnemyDetectFOV;

	
	//NearEnemy
public:
	void SetNearestEnemy(class AAssassinCharacter* Target);
protected:
	float NearEnemyRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	class AAssassinCharacter* NearestEnemy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

public:
	/**칼을 blocking 상태로 전환*/
	void EnableBlock();
	/**칼을 idle 상태로 전환*/
	void DisableBlock();
	/**현재 block상태를 return*/
	bool GetIsBlock();
	/**Parry를 시도*/
	void TryParry();
	UFUNCTION()
	void PlayParry(AAssassinCharacter* Enemy);

private:
	bool IsBlock;
public:
	int32 IsParry;
	int32 EnemyAttackSectionIndex;
public:
	bool CanParry;

};
