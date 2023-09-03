// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/RangeAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFrameworks/AssassinGameMode.h"

ARangeAIController::ARangeAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Script/AIModule.BehaviorTree'/Game/AMainDirectory/Enemy/AI/Range/BT_Range.BT_Range'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void ARangeAIController::ExcuteDetect()
{
	Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Attacking);
}
