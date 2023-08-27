// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BossExplosionSkill.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/AIControllerBase.h"
#include "Character/Enemy/Boss.h"

UBTTask_BossExplosionSkill::UBTTask_BossExplosionSkill()
{
	NodeName = TEXT("BossExplosionSkill");
}

EBTNodeResult::Type UBTTask_BossExplosionSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	 Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<ABoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	FVector SkillLoc = OwnerComp.GetBlackboardComponent()->GetValueAsVector("TargetLoc");
	Enemy->ExplosionSkill(SkillLoc);
	
	return EBTNodeResult::Succeeded;
}
