// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Patrol.h"

#include "AIController.h"
#include "AI/AIPatrol.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/AIControllerBase.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/MeleeEnemy.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol Path");
	bNotifyTick = true;
	CanMove = false;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	auto Enemy = Cast<AMeleeEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	FVector nextPoint = Enemy->AiPatrolComponent->GetNextLocaiton();
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(AMeleeAIController::TargetLocKey,nextPoint);
	
	if(Enemy->AiPatrolComponent->GetIsCheckPoint())
	{
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				this->CanMove = true;
				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), 5.f, false);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Succeeded;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	UE_LOG(LogTemp, Warning, TEXT("--------Doing Patrol!!---------"));
	if(CanMove)
	{
		return FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
