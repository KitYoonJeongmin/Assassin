// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AssassinCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API APlayerCharacter : public AAssassinCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	FVector GetInputRightDirection();
	FVector GetInputForwardDirection();
public:
	/**주변에 있는 적들을 찾는 함수*/
	TArray<class AEnemy*> DetectNearByEnemy(float SearchRadius);
	/**시야각에 있는 적들을 찾는 함수*/
	TArray<class AEnemy*> DetectNearByEnemyInViewAngle(float SearchRadius, float FOVAngle);
};
