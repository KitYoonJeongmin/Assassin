// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_BossAIState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ACAnimInstance.h"
#include "Character/Enemy/BossAIController.h"
#include "Character/Enemy/Enemy.h"

UBTService_BossAIState::UBTService_BossAIState()
{
	NodeName = TEXT("BossAIState");
	Interval = 1.0f;
}

void UBTService_BossAIState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* OwnerEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == OwnerEnemy) return;

	if(OwnerEnemy->ACAnim->GetCurrentActiveMontage() != nullptr) return;
	//거리에 따라 상태를 설정
	AAssassinCharacter* TargetChar = Cast<AAssassinCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIControllerBase::TargetKey));
	if(250.f<=OwnerEnemy->GetDistanceTo(TargetChar) && OwnerEnemy->GetDistanceTo(TargetChar)<=350.f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(ABossAIController::AIStateKey, (int8)EAIState::E_Attacking);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(ABossAIController::AIStateKey, (int8)EAIState::E_Holding);
	}
	
	//랜덤으로 상태를 설정
	/*
	int32 random = FMath::RandRange(0,3);

	if(random == 0)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(ABossAIController::AIStateKey, (int8)EAIState::E_Holding);  // 0.25 확률로 피하기
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(ABossAIController::AIStateKey, (int8)EAIState::E_Attacking);	// 0.75 확률로 공격
	}
	*/
}
