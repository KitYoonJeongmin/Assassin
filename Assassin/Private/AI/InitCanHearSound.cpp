// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/InitCanHearSound.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/MeleeAIController.h"

EBTNodeResult::Type UInitCanHearSound::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMeleeAIController::CanHearSoundKey,false);
	return EBTNodeResult::Succeeded;
}
