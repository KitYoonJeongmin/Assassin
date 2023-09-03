// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/RangeEnemy.h"

#include "AI/AIPatrol.h"
#include "Character/Enemy/RangeAIController.h"
#include "Weapons/Bow.h"

ARangeEnemy::ARangeEnemy()
{
	AiPatrolComponent = CreateDefaultSubobject<UAIPatrol>(TEXT("AIPATROLCOMPONENT"));
	//AI
	AIControllerClass = ARangeAIController::StaticClass();
}

void ARangeEnemy::BeginPlay()
{
	Super::BeginPlay();

	Weapon.BowWeapon = GetWorld()->SpawnActor<ABow>(FVector::ZeroVector, FRotator::ZeroRotator);
	AttachWeaponTo(Weapon.BowWeapon, FName("BowSocket"), false);
	Weapon.BowWeapon->InitializeWeapon(this);

	CurrentWeapon = Weapon.BowWeapon;

}
