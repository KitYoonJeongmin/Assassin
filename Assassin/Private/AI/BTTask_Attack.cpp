// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"

#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/Enemy.h"
#include "Character/ACAnimInstance.h"
#include "Weapons/Weapon.h"
#include "Weapons/Sword.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{

}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	if (Enemy->GetCurrentWeapon() == nullptr)	//무기를 장착하지 않았다면
	{
		if (Enemy->ACAnim->GetCurrentActiveMontage() != nullptr && !Enemy->ACAnim->GetCurrentActiveMontage()->GetName().Contains("EquipSword"))
		{
			Enemy->ACAnim->PlayEquipMontage(true);
		}
	}
	if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::IsAttackingKey) != Enemy)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		Enemy->Attack();
	}
	
	return EBTNodeResult::Succeeded;
}