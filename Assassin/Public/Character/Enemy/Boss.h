// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "Boss.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ABoss : public AEnemy
{
	GENERATED_BODY()
	ABoss();
public:
	virtual void BeginPlay() override;
	void LaunchSkill();
	void ExplosionSkill(FVector TargetVec);
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ExplosionSkill, meta = (AllowPrivateAccess = "true"))
	UClass* BossExplosionSkillClass;

};
