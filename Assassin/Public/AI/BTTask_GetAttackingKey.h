// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetAttackingKey.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTTask_GetAttackingKey : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_GetAttackingKey();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
