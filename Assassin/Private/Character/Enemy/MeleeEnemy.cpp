// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MeleeEnemy.h"

#include "AI/AIPatrol.h"
#include "Weapons/Sword.h"

AMeleeEnemy::AMeleeEnemy()
{
	AiPatrolComponent = CreateDefaultSubobject<UAIPatrol>(TEXT("AIPATROLCOMPONENT"));
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

