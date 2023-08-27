// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBTTask_PlayMontage : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_PlayMontage();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSecondes) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PlayMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class AEnemy* Enemy;
};
