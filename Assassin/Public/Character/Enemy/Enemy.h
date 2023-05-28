// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AssassinCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API AEnemy : public AAssassinCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void Dead() override;
	
};
