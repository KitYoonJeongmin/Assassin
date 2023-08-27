// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_PlayMontage.h"

#include "AIController.h"
#include "Character/ACAnimInstance.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"

UBTTask_PlayMontage::UBTTask_PlayMontage()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	if(!Enemy->ACAnim->Montage_IsPlaying(PlayMontage))
	{
		Enemy->ACAnim->Montage_Play(PlayMontage,1.f);
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTask_PlayMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSecondes)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSecondes);
	if (Cast<AMeleeAIController>(Enemy->GetController())->GetDetectionLevel() <= 0.f)
	{
		//Enemy->ACAnim->Montage_Stop(0.2f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	/*
	else
	{
		if(Enemy->ACAnim->GetCurrentActiveMontage() == nullptr && !Enemy->ACAnim->Montage_IsPlaying(PlayMontage))
		{
			Enemy->ACAnim->Montage_Play(PlayMontage,1.f);
		}
	}*/
}
