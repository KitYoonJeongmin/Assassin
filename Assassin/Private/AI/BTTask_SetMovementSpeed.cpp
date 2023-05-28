// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SetMovementSpeed.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = TEXT("MoveSpeed");
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	Enemy->SetWalkSpeed(MaxSpeed);
	return EBTNodeResult::Succeeded;

}
