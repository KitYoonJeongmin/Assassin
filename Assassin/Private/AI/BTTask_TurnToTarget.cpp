// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"

#include "Character/PlayerCharacter.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/Enemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	
	//auto Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::TargetKey));
	FVector TargetLoc;
	auto Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	if (Target)
	{
		TargetLoc = Target->GetActorLocation();
	}
	else
	{
		TargetLoc = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
	}

	//FVector LookVector = Target->GetActorLocation() - Enemy->GetActorLocation();
	FVector LookVector = TargetLoc - Enemy->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	Enemy->SetActorRotation(FMath::RInterpTo(Enemy->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 20.0f));

	return EBTNodeResult::Succeeded;
}