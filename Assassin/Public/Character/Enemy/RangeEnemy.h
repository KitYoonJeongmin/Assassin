// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "RangeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ARangeEnemy : public AEnemy
{
	GENERATED_BODY()
	ARangeEnemy();
public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	class UAIPatrol* AiPatrolComponent;
};
