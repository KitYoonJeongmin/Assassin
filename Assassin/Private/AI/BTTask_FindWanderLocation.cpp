// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindWanderLocation.h"
#include "Character/PlayerCharacter.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Character/Enemy/Enemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindWanderLocation::UBTTask_FindWanderLocation()
{
	NodeName = TEXT("FindWanderLoc");
}

EBTNodeResult::Type UBTTask_FindWanderLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("---Enemy null----"));
		return EBTNodeResult::Failed;
	}
		

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMeleeAIController::TargetKey));
	if (Target == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("---TargetNull----"));
		return EBTNodeResult::Failed;
	}
		

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(Enemy->GetWorld());
	if (nullptr == NavSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("----Nav Null---"));
		return EBTNodeResult::Failed;
	}
		
	
	FVector CenterLoc = Enemy->GetActorLocation();
	//if (Enemy->GetDistanceTo(Target) < LocationRadius+100.f)	//적과 타겟의 거리가 반경보다 작다면
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("----Close---"));
	//	FVector TargetLoc = Target->GetActorLocation();
	//	FVector TargetToEnemy = Enemy->GetActorLocation() - TargetLoc;
	//	TargetToEnemy.Z = 0;
	//	TargetToEnemy.Normalize();
	//	CenterLoc = Enemy->GetActorLocation() - TargetToEnemy * (LocationRadius+100.f - Enemy->GetDistanceTo(Target));
	//	LocationRadius = Enemy->GetDistanceTo(Target);
	//	
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("----JustDoit---"));
	//	FVector TargetLoc = Target->GetActorLocation();
	//	FVector TargetToEnemy = Enemy->GetActorLocation() - TargetLoc;

	//	TargetToEnemy.Z = 0;
	//	TargetToEnemy.Normalize();
	//	CenterLoc = TargetLoc + TargetToEnemy * (LocationRadius + 200.f);
	//	//CenterLoc = Enemy->GetActorLocation() + TargetToEnemy * (LocationRadius + 100.f);
	//}

	//DrawDebugSphere(GetWorld(), CenterLoc, LocationRadius, 50, FColor(181, 0, 0), false, 2.f);
	
	FNavLocation NextTargetLoc;
	NavSystem->GetRandomPointInNavigableRadius(CenterLoc, LocationRadius, NextTargetLoc);
	{
		//DrawDebugSphere(GetWorld(), NextTargetLoc, 20.f, 10, FColor(0, 181, 0), false, 2.f);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AMeleeAIController::TargetLocKey, NextTargetLoc.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
