// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Dodge.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTService_Dodge : public UBTService
{
	GENERATED_BODY()
	UBTService_Dodge();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Distance)
	float MaxDistance;
};
