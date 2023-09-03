// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MeleeEnemy.h"

#include "AI/AIPatrol.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Weapons/Sword.h"

AMeleeEnemy::AMeleeEnemy()
{
	
	AiPatrolComponent = CreateDefaultSubobject<UAIPatrol>(TEXT("AIPATROLCOMPONENT"));

	//AI
	AIControllerClass = AMeleeAIController::StaticClass();
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	Weapon.SwordWeapon = GetWorld()->SpawnActor<ASword>(FVector::ZeroVector, FRotator::ZeroRotator);
	AttachWeaponTo(Weapon.SwordWeapon, FName("SwordSocket"), false);
	Weapon.SwordWeapon->InitializeWeapon(this);
	SetEnemySwordAttackCollisionChannel();
}

