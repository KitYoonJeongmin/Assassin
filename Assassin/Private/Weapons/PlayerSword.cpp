// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PlayerSword.h"

#include "Character/PlayerCharacter.h"
#include "Character/AssassinCharacter.h"
#include "Character/Enemy/Enemy.h"
void APlayerSword::Attack()
{
	NearestEnemy = nullptr;
	TArray<AEnemy*> EnemyArray = Player->DetectNearByEnemyInViewAngle(EnemyDetectRange, EnemyDetectFOV);
	if (EnemyArray.Num() > 0)
	{
		NearestEnemy = Player->FindNearestEnemy(TArray<AAssassinCharacter*>(EnemyArray));
	}
	else
	{
		TArray<AAssassinCharacter*> ACArray = Player->DetectNearByEnemy(EnemyDetectRange);
		if (ACArray.Num() > 0)
		{
			NearestEnemy = Player->FindNearestEnemy(ACArray);
		}

	}
	Super::Attack();
}
