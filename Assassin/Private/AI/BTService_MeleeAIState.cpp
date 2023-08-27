// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_MeleeAIState.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
UBTService_MeleeAIState::UBTService_MeleeAIState()
{
	NodeName = TEXT("AIState");
	Interval = 1.0f;
}

void UBTService_MeleeAIState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AEnemy* OwnerEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == OwnerEnemy) return;

	if(OwnerComp.GetBlackboardComponent()->GetValueAsEnum(AMeleeAIController::AIStateKey) == (int8)EAIState::E_Warning) return;
	
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::TargetKey));
	if (Target == nullptr) return;

	bool CanAttackRange = OwnerEnemy->GetDistanceTo(Target) < AttackingRange;
	AEnemy* AttackingEnemy = Cast<AEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::IsAttackingKey));
	if (CanAttackRange && (AttackingEnemy == nullptr|| AttackingEnemy == OwnerEnemy))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMeleeAIController::IsAttackingKey, OwnerEnemy);
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(AMeleeAIController::AIStateKey, (int8)EAIState::E_Attacking);
	}
	else
	{
		if (Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::IsAttackingKey)) == OwnerEnemy)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMeleeAIController::IsAttackingKey, nullptr);
		}
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(AMeleeAIController::AIStateKey, (int8)EAIState::E_Holding);
	}
}
