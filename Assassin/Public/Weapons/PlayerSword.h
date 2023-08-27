// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Sword.h"
#include "PlayerSword.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API APlayerSword : public ASword
{
	GENERATED_BODY()
public:
	virtual void Attack() override;
};
