// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/BossAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ACAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "Character/Enemy/Enemy.h"

ABossAIController::ABossAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Script/AIModule.BlackboardData'/Game/AMainDirectory/Enemy/AI/Melee/BB_Melee.BB_Melee'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Script/AIModule.BehaviorTree'/Game/AMainDirectory/Enemy/AI/Boss/BT_Boss.BT_Boss'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void ABossAIController:: OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	if(OwnerEnemy == nullptr) return;
	if(OwnerEnemy->GetIsDead()) return;
	if(Blackboard->GetValueAsBool(IsDeadKey)) return;

	Sight(actor, Stimulus);
	if (!Stimulus.WasSuccessfullySensed())	//보던것, 듣던것을 잃음
	{
		if (Cast<APlayerCharacter>(actor))	//PlayerCharacter를 못볼 때
		{
			if(Stimulus.Type == 0)	//시각 정보를 잃었을 때
			{
				Blackboard->SetValueAsBool(CanSeePlayerKey, false);
				Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Idle);
			}
		}
	}
}
void ABossAIController::Sight(AActor* actor, FAIStimulus const Stimulus)
{
	if (Cast<APlayerCharacter>(actor))	//플레이어를 봤다면
	{
		
		Blackboard->SetValueAsObject(TargetKey, Cast<APlayerCharacter>(actor));

		AActor* FocusActor = actor; // 포커스를 설정할 대상 액터
		//EAIFocusPriority::Type Priority = FAIFocusPriority(1); // 포커스 우선순위

		SetFocus(FocusActor);
		Blackboard->SetValueAsBool(CanSeePlayerKey, true);
		Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Holding);
		OwnerEnemy->ACAnim->PlayEquipMontage(true);
	}
}