// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetAttackingKey.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/Enemy.h"
#include "Character/ACAnimInstance.h"
#include "Weapons/Weapon.h"
#include "Weapons/Sword.h"

#include "BehaviorTree/BlackboardComponent.h"
UBTTask_GetAttackingKey::UBTTask_GetAttackingKey()
{

}

EBTNodeResult::Type UBTTask_GetAttackingKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	
	if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::IsAttackingKey)!=nullptr)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMeleeAIController::IsAttackingKey, Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn()));
	}
	return EBTNodeResult::Succeeded;
}