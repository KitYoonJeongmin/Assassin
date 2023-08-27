// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "MeleeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API AMeleeEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AMeleeEnemy();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	class UAIPatrol* AiPatrolComponent;
};
