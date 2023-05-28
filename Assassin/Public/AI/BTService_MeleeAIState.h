// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_MeleeAIState.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTService_MeleeAIState : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_MeleeAIState();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackingRange;
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
