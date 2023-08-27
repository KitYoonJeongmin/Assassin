// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Block.h"

#include "AIController.h"
#include "Character/Enemy/Enemy.h"
#include "Weapons/Sword.h"

UBTService_Block::UBTService_Block()
{
	NodeName = TEXT("Block");
	Interval = 1.0f;
}

void UBTService_Block::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* OwnerEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == OwnerEnemy) return;

	/*
	int32 random = FMath::RandRange(0,2);
	if(random == 0)
	{
	OwnerEnemy->Weapon.SwordWeapon->IsParry = true;
	}
	else
	{
		OwnerEnemy->Weapon.SwordWeapon->IsParry = false;
	}
	*/
	//OwnerEnemy->Weapon.SwordWeapon->IsParry = true;
	OwnerEnemy->Weapon.SwordWeapon->TryParry();
	
}
