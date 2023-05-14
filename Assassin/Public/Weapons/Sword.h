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
	virtual void InitializeWeapon() override;

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
	/**콤보 공격을 실행하는 함수*/
	virtual void Attack() override;

	UFUNCTION()
	void OnEnableAttackCheck();
	UFUNCTION()
	void OnDisableAttackCheck();
	UFUNCTION()
	void OnNextAttackCheck();

	
private:
	bool CanNextCombo;
	bool CanAttackCheck;
	bool IsAttacking;
	bool IsComboInputOn;
	int32 CurrentCombo;
	FVector AttackHitVec;
	TArray<AActor*> ActorsToIgnore;

	float EnemyDetectRange;
	float EnemyDetectFOV;

	//NearEnemy
public:
	class AEnemy* FindNearestEnemey(TArray<class AEnemy*> EnemyArr);
private:
	float NearEnemyRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	class AEnemy* NearestEnemy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

	
};
