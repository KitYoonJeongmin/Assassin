// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BossAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ACAnimInstance.h"
#include "Character/Enemy/AIControllerBase.h"
#include "Character/Enemy/Boss.h"

UBTTask_BossAttack::UBTTask_BossAttack()
{
	
}

EBTNodeResult::Type UBTTask_BossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto Enemy = Cast<ABoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	if (Enemy->GetCurrentWeapon() == nullptr)	//무기를 장착하지 않았다면
	{
		if (Enemy->ACAnim->GetCurrentActiveMontage() != nullptr && !Enemy->ACAnim->GetCurrentActiveMontage()->GetName().Contains("EquipSword"))
		{
			Enemy->ACAnim->PlayEquipMontage(true);
		}
	}
	AAssassinCharacter* TargetEnemy = Cast<AAssassinCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIControllerBase::TargetKey));
	if(TargetEnemy->GetDistanceTo(Enemy)<240.f)
	{
		Enemy->ACAnim->PlayLaunchSkillMontage();
	}
	else
	{
		Enemy->Attack();
	}
	return EBTNodeResult::Succeeded;
}
