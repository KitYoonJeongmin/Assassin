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

#include "DetectionWidget.h"
#include "GameFrameworks/AssassinGameMode.h"
#include "Kismet/GameplayStatics.h"



const FName AMeleeAIController::CanHearSoundKey(TEXT("CanHearSound"));
const FName AMeleeAIController::TargetLocKey(TEXT("TargetLoc"));
const FName AMeleeAIController::IsAttackingKey(TEXT("IsAttacking"));

const FName AMeleeAIController::LastTargetLocKey(TEXT("LastTargetLoc"));


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

	
	//AI Perception::Hearing 설정
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

	HearingConfig->HearingRange = 3000.f;	//범위 설정
	HearingConfig->LoSHearingRange = 3000.f;	//범위 디버거
	HearingConfig->SetMaxAge(5.f);

	HearingConfig->DetectionByAffiliation.bDetectEnemies = true; // 소속별 탐지 적
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true; // 소속별 탐지 팀
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true; // 소속별 탐지 중립
	
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);	//감각 추가

	IsIncreaseDetectionLevel = false;	//감지를 증가하지 않도록 설정
	DetectionLevel = 0.f; //detection level을 0으로 설정
	CanSense = true;	//감각을 느낄 수 있는가
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
		Blackboard->SetValueAsBool(CanHearSoundKey, false);
		
	}
}

void AMeleeAIController::BeginPlay()
{
	Super::BeginPlay();
	OwnerEnemy->ACAnim->OnFinished.AddDynamic(this, &AMeleeAIController::SetDead);
	OwnerEnemy->ACAnim->OnMontageEnded.AddDynamic(this, &AMeleeAIController::OnAttackMontageEnded);
	OwnerEnemy->ACAnim->OnMontageStarted.AddDynamic(this, &AMeleeAIController::OnAttackMontageStarted);
}

void AMeleeAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateDetectionLevel();
}

void AMeleeAIController:: OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	if(OwnerEnemy->GetIsDead()) return;
	if(Blackboard->GetValueAsBool(IsDeadKey)) return;
	if(!CanSense) return;

	switch (Stimulus.Type)
	{
	case 0:	//Sight
		Sight(actor, Stimulus);
		break;
	case 1:	//Hearing
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Sound")));
		Hearing(actor, Stimulus);
		break;
	}
	if (!Stimulus.WasSuccessfullySensed())	//보던것, 듣던것을 잃음
	{
		
		if (Cast<APlayerCharacter>(actor))	//PlayerCharacter를 못볼 때
		{
			if(Stimulus.Type == 0)	//시각 정보를 잃었을 때
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy Lose Sight"));
				LoseSense();
			}
			
		}
	}
}

void AMeleeAIController::Sight(AActor* actor, FAIStimulus const Stimulus)
{
	if (Cast<APlayerCharacter>(actor))	//플레이어를 봤다면
	{
		OwnerEnemy->ACAnim->Montage_Stop(0.2f);
		Blackboard->SetValueAsObject(TargetKey, Cast<APlayerCharacter>(actor));

		AActor* FocusActor = actor; // 포커스를 설정할 대상 액터
		//EAIFocusPriority::Type Priority = FAIFocusPriority(1); // 포커스 우선순위

		SetFocus(FocusActor);
		Blackboard->SetValueAsBool(CanSeePlayerKey, true);
		Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Warning);
		IsIncreaseDetectionLevel = true;//감지레벨 증가
		return;
	}
}


void AMeleeAIController::Hearing(AActor* actor, FAIStimulus const Stimulus)
{
	Blackboard->SetValueAsBool(CanHearSoundKey, true);
	Blackboard->SetValueAsVector(TargetLocKey, actor->GetActorLocation());
}

void AMeleeAIController::LoseSense()
{
	//칼 넣기
	if(OwnerEnemy->GetCurrentWeapon() == OwnerEnemy->Weapon.SwordWeapon)
	{
		OwnerEnemy->ACAnim->PlayEquipMontage(false);
	}
	

	//BT값 초기화
	if(Cast<AActor>(Blackboard->GetValueAsObject(TargetKey)))
	{
		Blackboard->SetValueAsVector(LastTargetLocKey, Cast<AActor>(Blackboard->GetValueAsObject(TargetKey))->GetActorLocation());
	}
	Blackboard->SetValueAsBool(CanSeePlayerKey, false);
	Blackboard->SetValueAsObject(TargetKey, nullptr);
	Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Warning);
	SetFocus(nullptr);
	
	//감지레벨 감소
	IsIncreaseDetectionLevel = false;

	//게임 모드의 전투 적 수 감소
	if(DetectionLevel >=100.f)
	{
		Cast<AAssassinGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CountCombatEnemyNum(false);
	}
}

void AMeleeAIController::SetDead()
{
	Super::SetDead();
	if(OwnerEnemy->GetIsDead()) return;
	IsIncreaseDetectionLevel = false;
	
	if(DetectionLevel >=100.f)
	{
		Cast<AAssassinGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CountCombatEnemyNum(false);
	}
	
}

void AMeleeAIController::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName().Contains("Attack"))
	{
		Blackboard->SetValueAsObject(AMeleeAIController::IsAttackingKey, nullptr);
	}
	if (Montage->GetName().Contains("Vic"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("-------End Finishing ------"));
		Blackboard->SetValueAsObject(AMeleeAIController::IsAttackingKey, nullptr);
	}
}

void AMeleeAIController::OnAttackMontageStarted(UAnimMontage* Montage)
{
	if (Montage->GetName().Contains("Vic"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("-------Start Finishing ------"));
		Blackboard->SetValueAsObject(AMeleeAIController::IsAttackingKey, OwnerEnemy);
	}
}

void AMeleeAIController::UpdateDetectionLevel()
{
	if(OwnerEnemy->GetIsDead()) return;
	if(IsIncreaseDetectionLevel && DetectionLevel<100.f)	//증가
	{
		DetectionLevel += 0.25f;
		OwnerEnemy->EnableDetectionWidget(true);
		if(DetectionLevel >= 100.f && OwnerEnemy->GetCurrentWeapon() != OwnerEnemy->Weapon.SwordWeapon)
		{
			/*
			//칼 착용
			OwnerEnemy->ACAnim->PlayEquipMontage(true);
			Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Holding);
			//UE_LOG(LogTemp, Warning, TEXT("Equip Sword!!"));

			Cast<AAssassinGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CountCombatEnemyNum(true);
			*/
			ExcuteDetect();
		}
	}
	else if(!IsIncreaseDetectionLevel &&DetectionLevel>0.f)	//감소
	{
		DetectionLevel -= 0.25f;
		if(DetectionLevel<=0.f)
		{
			OwnerEnemy->EnableDetectionWidget(false);
			Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Idle);
		}
	}
	DetectionLevel = FMath::Clamp(DetectionLevel,0.f,100.f);
	OwnerEnemy->UpdateDetectionWidget(DetectionLevel);
}

float AMeleeAIController::GetDetectionLevel() const
{
	return DetectionLevel;
}

void AMeleeAIController::ExcuteDetect()
{
	//칼 착용
	OwnerEnemy->ACAnim->PlayEquipMontage(true);
	Blackboard->SetValueAsEnum(AIStateKey, (int8)EAIState::E_Holding);
	//UE_LOG(LogTemp, Warning, TEXT("Equip Sword!!"));

	Cast<AAssassinGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->CountCombatEnemyNum(true);
}



