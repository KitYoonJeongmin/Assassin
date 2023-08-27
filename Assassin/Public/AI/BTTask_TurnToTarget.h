// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTTask_TurnToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
