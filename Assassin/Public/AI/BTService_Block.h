// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Block.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTService_Block : public UBTService
{
	GENERATED_BODY()
	UBTService_Block();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
