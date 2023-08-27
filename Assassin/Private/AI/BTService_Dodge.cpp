// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Dodge.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ACAnimInstance.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"

UBTService_Dodge::UBTService_Dodge()
{
	NodeName = TEXT("Dodge");
	Interval = 1.0f;
	MaxDistance = 230.f;
}

void UBTService_Dodge::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AEnemy* OwnerEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerEnemy == nullptr) return;

	AAssassinCharacter* TargetCharacter = Cast<AAssassinCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIControllerBase::TargetKey));
	if(TargetCharacter == nullptr) return;

	float Distance = OwnerEnemy->GetDistanceTo(TargetCharacter);
	if(Distance<MaxDistance)
	{
		OwnerEnemy->ACAnim->PlaySwordRollMontage();
	}
	
}
