// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/AIControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

const FName AAIControllerBase::CanSeePlayerKey(TEXT("CanSeePlayer"));
const FName AAIControllerBase::TargetKey(TEXT("Target"));
const FName AAIControllerBase::IsStunKey(TEXT("IsStun"));
const FName AAIControllerBase::IsDeadKey(TEXT("IsDead"));
const FName AAIControllerBase::AIStateKey(TEXT("AIState"));

AAIControllerBase::AAIControllerBase()
{
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = AISightRadius; // 시각 반경
	SightConfig->LoseSightRadius = SightConfig->SightRadius + AILoseSightRadius; // 시각 상실 반경
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView; // 주변 시야각
	SightConfig->SetMaxAge(AISightAge); //자극이 잊히기 까지의 시간 (0이면 잊지않음)
	SightConfig->AutoSuccessRangeFromLastSeenLocation = AILastSeenLocation;	//마지막으로 본 위치의 해당 범위내에 있으면 이미 본 대상을 항상 볼 수 있음

	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 소속별 탐지 적
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // 소속별 탐지 팀
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // 소속별 탐지 중립

	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnTargetDetected); //지정된 대상에 대해 인식 정보가 업데이트되었음을 ​​바인딩된 모든 개체에 알림

}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UBlackboardComponent* BlackboardComp = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardComp))
	{
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("AIController couldn't run behavior tree!"));
		}
		Blackboard = BlackboardComp;
		Blackboard->SetValueAsBool(CanSeePlayerKey, false);
		Blackboard->SetValueAsBool(IsDeadKey, false);
		Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Idle);
		if (Cast<AEnemy>(InPawn))
		{
			OwnerEnemy = Cast<AEnemy>(InPawn);
			OwnerEnemy->OnStunStart.AddDynamic(this, &AAIControllerBase::OnDisableBT);
			OwnerEnemy->OnStunEnd.AddDynamic(this, &AAIControllerBase::OnEnableBT);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't possess to Enemy"));
		}
	}
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAIControllerBase::SetDead()
{
	if(OwnerEnemy->GetIsDead()) return;
	OwnerEnemy->EnableDetectionWidget(false);
	Blackboard->SetValueAsBool(IsDeadKey, true);
}

void AAIControllerBase::OnDisableBT()
{
	Blackboard->SetValueAsBool(AAIControllerBase::IsStunKey, true);

}

void AAIControllerBase::OnEnableBT()
{
	Blackboard->SetValueAsBool(AAIControllerBase::IsStunKey, false);
}