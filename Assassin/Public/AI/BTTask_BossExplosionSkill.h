// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossExplosionSkill.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTTask_BossExplosionSkill : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_BossExplosionSkill();
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
  