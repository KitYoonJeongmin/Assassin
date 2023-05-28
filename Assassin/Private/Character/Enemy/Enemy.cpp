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
}
void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ACAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnAttackMontageEnded);
}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (Weapon.SwordWeapon != nullptr)
	{
		Weapon.SwordWeapon->SwordAttackType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3);
	}
}
void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnAttackMontageEnded(Montage, bInterrupted);
	
}
void AEnemy::Dead()
{
	Super::Dead();
	if (Cast<AMeleeAIController>(GetController()) != nullptr)
	{
		Cast<AMeleeAIController>(GetController())->SetDead();
	}
}
