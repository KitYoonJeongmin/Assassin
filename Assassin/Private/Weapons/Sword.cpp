// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Sword.h"
#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Character/Enemy.h"
#include "Character/ACAnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
ASword::ASword()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Script/Engine.StaticMesh'/Game/MedievalArmour/CharacterParts/Meshes/StaticMeshes/SM_wp_sword_02.SM_wp_sword_02'"));
	if (SM_WEAPON.Succeeded())
	{
		Mesh->SetStaticMesh(SM_WEAPON.Object);
	}

	//Attack
	IsAttacking = false;
	WeaponDamage = 20.f;
	CanAttackCheck = false;
	AttackEndComboState();

	//Detect Enemy
	EnemyDetectRange = 600.f;
	EnemyDetectFOV = 75.f;
}

void ASword::BeginPlay()
{
	Super::BeginPlay();
}

void ASword::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AttackCheck();
	
	DrawDebugDirectionalArrow(GetWorld(),GetActorLocation() + GetActorUpVector() * 50.f, GetActorLocation() + GetActorUpVector() * 50.f - GetActorForwardVector()*50.f, 50.f, FColor::Magenta);
}

void ASword::InitializeWeapon()
{
	Super::InitializeWeapon();
	Character->ACAnim->OnMontageEnded.AddDynamic(this, &ASword::OnAttackMontageEnded);
	Character->ACAnim->OnNextAttackCheck.AddDynamic(this, &ASword::OnNextAttackCheck);
	Character->ACAnim->OnEnableAttackHitCheck.AddDynamic(this, &ASword::OnEnableAttackCheck);
	Character->ACAnim->OnDisableAttackHitCheck.AddDynamic(this, &ASword::OnDisableAttackCheck);
	Player = Cast<APlayerCharacter>(Character);
}

void ASword::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CurrentCombo += 1;
}

void ASword::AttackEndComboState()
{
	CanNextCombo = false;
	IsComboInputOn = false;
	CurrentCombo = 0;
}

void ASword::AttackCheck()
{
	if (!CanAttackCheck) return;
	FVector StartLoc;
	FVector EndLoc;
	
	StartLoc = GetActorLocation() - GetActorUpVector() * 30.f;
	EndLoc = StartLoc + GetActorUpVector()*150.f;

	FHitResult HitResult;
	
	ActorsToIgnore.Add(Character);
	ActorsToIgnore.Add(this);

	bool AttackSuccess = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
		, FLinearColor::Gray
		, FLinearColor::Blue
		, 3.0f
	);

	if (AttackSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage!"));
		FDamageEvent DamageEvent;
		if (CurrentCombo == 4)
		{
			HitResult.GetActor()->TakeDamage(WeaponDamage * 2, DamageEvent, Character->GetController(), Character);
		}
		else
		{
			HitResult.GetActor()->TakeDamage(WeaponDamage, DamageEvent, Character->GetController(), Character);
		}
		ActorsToIgnore.Add(HitResult.GetActor());

	}
}

void ASword::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
	if ((Montage->GetName()).Contains("AttackCombo"))
	{
		IsAttacking = false;
		AttackEndComboState();
		CanAttackCheck = false;
		UE_LOG(LogTemp, Warning, TEXT("%s:Test for Delegate!"),*(Character->GetName()));
	}
}

void ASword::Attack()
{
	NearestEnemy = nullptr;
	if (IsAttacking)
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		if (CurrentCombo != 0) { UE_LOG(LogTemp, Warning, TEXT("Combo num is not zero!!")); return; }
		
		//State
		AttackStartComboState();
		IsAttacking = true;
		 
		//Animation
		Character->ACAnim->PlaySwordAttackMontage();
		Character->ACAnim->JumpToSwordAttackMontageSection(CurrentCombo);
	}
	if (Player != nullptr)
	{
		TArray<AEnemy*> EnemyArray = Player->DetectNearByEnemyInViewAngle(EnemyDetectRange, EnemyDetectFOV);
		if (EnemyArray.Num() > 0)
		{
			NearestEnemy = FindNearestEnemey(EnemyArray);
			DrawDebugSphere(GetWorld(), NearestEnemy->GetActorLocation(), 30.f, 10, FColor(181, 0, 0));
		}
	}
}



void ASword::OnEnableAttackCheck()
{
	CanAttackCheck = true;
	ActorsToIgnore.Empty();

	//Rot
	if (NearestEnemy != nullptr)
	{
		FVector FrontFromEnemy = NearestEnemy->GetActorLocation();
		FVector PlayerToEnemyDirection = NearestEnemy->GetActorLocation() - Player->GetActorLocation();
		PlayerToEnemyDirection.Z = 0;
		PlayerToEnemyDirection.Normalize();
		FrontFromEnemy -= PlayerToEnemyDirection * 66.f;
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), NearestEnemy->GetActorLocation());
		LookAtRot.Pitch = Character->GetActorRotation().Pitch;
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(Character->GetCapsuleComponent(), FrontFromEnemy, LookAtRot, false, false, 0.15f, false, EMoveComponentAction::Type::Move, Info);

	}
}
void ASword::OnDisableAttackCheck()
{
	CanAttackCheck = false;
}
void ASword::OnNextAttackCheck()
{
	CanNextCombo = false;
	if (IsComboInputOn)
	{
		AttackStartComboState();
		Character->ACAnim->JumpToSwordAttackMontageSection(CurrentCombo);
	}
}

AEnemy* ASword::FindNearestEnemey(TArray<class AEnemy*> EnemyArr)
{
	AEnemy* NearestEnemyResult = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	for (AEnemy* Enemy : EnemyArr)
	{
		// 현재 위치와 거리 계산
		float Distance = FVector::Dist(Enemy->GetActorLocation(), Character->GetActorLocation());

		// 가장 가까운 적인지 확인
		if (Distance < MinDistance)
		{
			NearestEnemyResult = Enemy;
			MinDistance = Distance;
		}
	}
	return NearestEnemyResult;
}


