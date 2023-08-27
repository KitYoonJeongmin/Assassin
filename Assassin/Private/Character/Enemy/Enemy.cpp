// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Character/ACAnimInstance.h"
#include "Weapons/Sword.h"
#include "GameFramework/CharacterMovementComponent.h"


AEnemy::AEnemy()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	//RVO Avoidance
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 700.f;
	//AI
	AIControllerClass = AMeleeAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//Health
	HealthPoint = 100.f;

	// DetectType
	DetectEnemyTrace = UEngineTypes::ConvertToTraceType(ECC_EngineTraceChannel1);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	//Character Weapons Spawn  Sword
	Weapon.SwordWeapon = GetWorld()->SpawnActor<ASword>(FVector::ZeroVector, FRotator::ZeroRotator);
	AttachWeaponTo(Weapon.SwordWeapon, FName("SwordSocket"), false);
	Weapon.SwordWeapon->InitializeWeapon(this);
	SetEnemySwordAttackCollisionChannel();
}

void AEnemy::Dead()
{
	Super::Dead();
	if (Cast<AAIControllerBase>(GetController()) != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("--------death---------"));
		Cast<AAIControllerBase>(GetController())->SetDead();
	}
}



void AEnemy::SetEnemySwordAttackCollisionChannel()
{
	if (Weapon.SwordWeapon != nullptr)
	{
		Weapon.SwordWeapon->SwordAttackType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3);
	}
}
