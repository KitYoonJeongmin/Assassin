// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RangeAttack.h"

#include "AIController.h"
#include "Character/Enemy/Enemy.h"
#include "Weapons/Weapon.h"
#include "Weapons/Bow.h"

UBTTask_RangeAttack::UBTTask_RangeAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;
	if(Enemy->GetCurrentWeapon() == nullptr || Enemy->GetCurrentWeapon() != Enemy->Weapon.BowWeapon) return EBTNodeResult::Failed;

	Enemy->Attack();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_RangeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if(Enemy->GetCurrentWeapon() && !Enemy->GetCurrentWeapon()->GetIsAttacking())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
