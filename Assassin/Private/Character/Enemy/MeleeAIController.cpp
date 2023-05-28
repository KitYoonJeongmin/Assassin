// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/Enemy.h"
#include "Character/PlayerCharacter.h"
#include "Character//ACAnimInstance.h"

#include "Weapons/Sword.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "..\..\..\Public\Character\Enemy\MeleeAIController.h"


const FName AMeleeAIController::CanSeePlayerKey(TEXT("CanSeePlayer"));
const FName AMeleeAIController::TargetKey(TEXT("Target"));
const FName AMeleeAIController::TargetLocKey(TEXT("TargetLoc"));
const FName AMeleeAIController::IsStunKey(TEXT("IsStun"));
const FName AMeleeAIController::IsDeadKey(TEXT("IsDead"));
const FName AMeleeAIController::IsAttackingKey(TEXT("IsAttacking"));
const FName AMeleeAIController::AIStateKey(TEXT("AIState"));



AMeleeAIController::AMeleeAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Script/AIModule.BlackboardData'/Game/AMainDirectory/Enemy/AI/Melee/BB_Melee.BB_Melee'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Script/AIModule.BehaviorTree'/Game/AMainDirectory/Enemy/AI/Melee/BT_Melee.BT_Melee'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

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
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AMeleeAIController::OnTargetDetected); //지정된 대상에 대해 인식 정보가 업데이트되었음을 ​​바인딩된 모든 개체에 알림

	//AI Perception::Hearing 설정
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

	HearingConfig->HearingRange = 3000.f;	//범위 설정
	HearingConfig->LoSHearingRange = 3000.f;	//범위 디버거
	HearingConfig->SetMaxAge(5.f);

	GetPerceptionComponent()->ConfigureSense(*HearingConfig);	//감각 추가
}

void AMeleeAIController::OnPossess(APawn* InPawn)
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
		Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Holding);
		if (Cast<AEnemy>(InPawn))
		{
			OwnerEnemy = Cast<AEnemy>(InPawn);
			OwnerEnemy->OnStunStart.AddDynamic(this, &AMeleeAIController::OnDisableBT);
			OwnerEnemy->OnStunEnd.AddDynamic(this, &AMeleeAIController::OnEnableBT);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't possess to Enemy"));
		}
	}
}

void AMeleeAIController::BeginPlay()
{
	Super::BeginPlay();
	OwnerEnemy->ACAnim->OnFinished.AddDynamic(this, &AMeleeAIController::SetDead);
	OwnerEnemy->ACAnim->OnMontageEnded.AddDynamic(this, &AMeleeAIController::OnAttackMontageEnded);
	OwnerEnemy->ACAnim->OnMontageStarted.AddDynamic(this, &AMeleeAIController::OnAttackMontageStarted);

	//OwnerEnemy->ACAnim->OnMontageStarted
}

void AMeleeAIController::OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	switch (Stimulus.Type)
	{
	case 0:	//Sight
		Sight(actor, Stimulus);
		break;
	case 1:	//Hearing
		Hearing(actor, Stimulus);
		break;
	}
	if (!Stimulus.WasSuccessfullySensed())	//보던것, 듣던것을 잃음
	{
		if (Cast<APlayerCharacter>(actor))	//PlayerCharacter를 못볼 때
		{
			//칼 넣기
			OwnerEnemy->ACAnim->PlayEquipMontage(false);

			//BT값 초기화
			Blackboard->SetValueAsBool(CanSeePlayerKey, false);
			Blackboard->SetValueAsObject(TargetKey, nullptr);
		}
	}
}

void AMeleeAIController::Sight(AActor* actor, FAIStimulus const Stimulus)
{
	if (Cast<APlayerCharacter>(actor))	//플레이어를 봤다면
	{
		//칼 착용
		OwnerEnemy->ACAnim->PlayEquipMontage(true);
		/*ASword* Sword = Cast<ASword>(OwnerEnemy->Weapon.SwordWeapon);
		if (Sword != nullptr)
		{
			Sword->SetNearestEnemy(Cast<AAssassinCharacter>(actor));
			UE_LOG(LogTemp, Warning, TEXT("Sword is not null"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Sword is null"));
		}*/
		Blackboard->SetValueAsBool(CanSeePlayerKey, true);
		Blackboard->SetValueAsObject(TargetKey, Cast<APlayerCharacter>(actor));

		AActor* FocusActor = actor; // 포커스를 설정할 대상 액터
		//EAIFocusPriority::Type Priority = FAIFocusPriority(1); // 포커스 우선순위

		SetFocus(FocusActor);
		return;
	}
}


void AMeleeAIController::Hearing(AActor* actor, FAIStimulus const Stimulus)
{

}

void AMeleeAIController::OnDisableBT()
{
	Blackboard->SetValueAsBool(IsStunKey, true);

}

void AMeleeAIController::OnEnableBT()
{
	Blackboard->SetValueAsBool(IsStunKey, false);
}

void AMeleeAIController::SetDead()
{
	Blackboard->SetValueAsBool(IsDeadKey, true);
}

void AMeleeAIController::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName().Contains("Attack"))
	{
		Blackboard->SetValueAsObject(AMeleeAIController::IsAttackingKey, nullptr);
	}
	if (Montage->GetName().Contains("Vic"))
	{
		UE_LOG(LogTemp, Warning, TEXT("-------End Finishing ------"));
		Blackboard->SetValueAsObject(AMeleeAIController::IsAttackingKey, nullptr);
	}
}

void AMeleeAIController::OnAttackMontageStarted(UAnimMontage* Montage)
{
	if (Montage->GetName().Contains("Vic"))
	{
		UE_LOG(LogTemp, Warning, TEXT("-------Start Finishing ------"));
		Blackboard->SetValueAsObject(AMeleeAIController::IsAttackingKey, OwnerEnemy);
	}
}