// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Sword.h"
#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Character/Enemy/Enemy.h"
#include "Character/ACAnimInstance.h"
#include "Component/FinisherComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

ASword::ASword()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Script/Engine.StaticMesh'/Game/SwordAnimsetPro/Models/Sword/Sword.Sword'"));
	if (SM_WEAPON.Succeeded())
	{
		Mesh->SetStaticMesh(SM_WEAPON.Object);
	}

	//Attack
	SwordAttackType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

	WeaponDamage = 20.f;

	CurrentCombo = 0;
	AttackEndComboState();

	//Detect Enemy
	EnemyDetectRange = 350.f;
	EnemyDetectFOV = 75.f;

	//Block
	IsBlock = false;
	
	//Parry
	IsParry = 0;
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

	//DrawDebugDirectionalArrow(GetWorld(),GetActorLocation() + GetActorUpVector() * 50.f, GetActorLocation() + GetActorUpVector() * 50.f - GetActorForwardVector()*50.f, 50.f, FColor::Magenta);
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
	CurrentCombo %= 5;
	CurrentCombo += 1;
	
}

void ASword::AttackEndComboState()
{
	CanNextCombo = false;
	IsComboInputOn = false;
	IsAttacking = false;
	CanAttackCheck = false;
	CanParry = false;
}

void ASword::AttackCheck()
{
	if (!CanAttackCheck) return;
	FVector StartLoc;
	FVector EndLoc;
	
	StartLoc = Mesh->GetSocketLocation(FName("Start"));
	EndLoc = Mesh->GetSocketLocation(FName("End"));

	FHitResult HitResult;
	
	ActorsToIgnore.Add(Character);
	ActorsToIgnore.Add(this);


	bool AttackSuccess = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		SwordAttackType,
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
		CanParry = false;
		//UE_LOG(LogTemp, Warning, TEXT("TakeDamage!"));
		FDamageEvent DamageEvent;
		HitResult.GetActor()->TakeDamage(WeaponDamage, DamageEvent, Character->GetController(), this);
		ActorsToIgnore.Add(HitResult.GetActor());

	}
}

void ASword::MoveToNearestEnemy()
{
	if (NearestEnemy == nullptr) return;
	FVector FrontFromEnemy = NearestEnemy->GetActorLocation();
	FVector CharacterToEnemyDirection = NearestEnemy->GetActorLocation() - Character->GetActorLocation();
	CharacterToEnemyDirection.Z = 0;
	CharacterToEnemyDirection.Normalize();
	FrontFromEnemy -= CharacterToEnemyDirection * 120.f;
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), NearestEnemy->GetActorLocation());
	LookAtRot.Pitch = Character->GetActorRotation().Pitch;
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(Character->GetCapsuleComponent(), FrontFromEnemy, LookAtRot, false, false, 0.2f, false, EMoveComponentAction::Type::Move, Info);

}

void ASword::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ComboAttackReset
	if (Montage->GetName().Contains("Combo"))
	{
		
		AttackEndComboState();
	}
}

void ASword::Attack()
{
	if (Player != nullptr)
	{
		NearestEnemy = nullptr;
		TArray<AEnemy*> EnemyArray = Player->DetectNearByEnemyInViewAngle(EnemyDetectRange, EnemyDetectFOV);
		if (EnemyArray.Num() > 0)
		{
			NearestEnemy = Player->FindNearestEnemy(EnemyArray);
			//DrawDebugSphere(GetWorld(), NearestEnemy->GetActorLocation(), 30.f, 10, FColor(181, 0, 0));
		}
		else
		{
			EnemyArray = Player->DetectNearByEnemy(EnemyDetectRange);
			if (EnemyArray.Num() > 0)
			{
				NearestEnemy = Player->FindNearestEnemy(EnemyArray);
				//DrawDebugSphere(GetWorld(), NearestEnemy->GetActorLocation(), 30.f, 10, FColor(181, 0, 0));
			}

		}
		
	}
	
	if (IsAttacking)
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		if (NearestEnemy != nullptr && NearestEnemy->GetHealthPoint() < 30.f)
		{
			TArray<AEnemy*> Enemys = Player->DetectNearByEnemy(300.f);
			/*int32 EnemyNum = 0;
			for (auto enemy : Enemys)
			{
				EnemyNum++;
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("%s"), *(enemy->GetName())));
				DrawDebugSphere(GetWorld(), enemy->GetActorLocation(), 30.f, 10, FColor(181, 0, 0),false,3.f);
			}
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("Near Enemy Number: %d"), EnemyNum));*/
			if (Enemys.Num() <= 1)
			{
				FinisherComp->PlayFinish(Character, NearestEnemy);
				return;
			}
		}

		//State
		AttackStartComboState();
		IsAttacking = true;
		CanParry = true;
		 
		//Animation
		Character->ACAnim->PlaySwordAttackMontage();
		Character->ACAnim->JumpToSwordAttackMontageSection(CurrentCombo);

	}
	
}

void ASword::OnEnableAttackCheck()
{
	//Move
	MoveToNearestEnemy();
	CanAttackCheck = true;
	ActorsToIgnore.Empty();
}
void ASword::OnDisableAttackCheck()
{
	CanAttackCheck = false;
}
void ASword::OnNextAttackCheck()
{
	CanNextCombo = false;
	if (NearestEnemy != nullptr)
	{
		if (NearestEnemy->GetHealthPoint() < 30.f)
		{
			TArray<AEnemy*> Enemys = Player->DetectNearByEnemy(300.f);
			
			/*for (auto enemy : Enemys)
			{
				
				DrawDebugSphere(GetWorld(), enemy->GetActorLocation(), 30.f, 10, FColor(181, 0, 0), false, 3.f);
			}
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("Near Enemy Number: %d"), EnemyNum));*/
			if (Enemys.Num() <=1)
			{
				FinisherComp->PlayFinish(Character, NearestEnemy);
				return;
			}
		}
	}
	
	if (IsComboInputOn)
	{
		AttackStartComboState();	//ComboState
		CanParry = true;	//상대방이Parry 가능
		Character->ACAnim->JumpToSwordAttackMontageSection(CurrentCombo);	//다음 Section으로 이동
	}
}


void ASword::SetNearestEnemy(AAssassinCharacter* Target)
{
	NearestEnemy = Target;
}

void ASword::EnableBlock()
{
	IsBlock = true;
	Character->ACAnim->IsBlock = true;
}

void ASword::DisableBlock()
{
	IsBlock = false;
	Character->ACAnim->IsBlock = false;
}

bool ASword::GetIsBlock()
{
	return IsBlock;
}

void ASword::TryParry()
{

	if (Player == nullptr) return;
	TArray<AEnemy*> EnemyArr = Player->DetectNearByEnemy(250.f);
	for (auto Enemy : EnemyArr)
	{
		//적이 칼을 가지고있고 패링할 수 있는 상태라면
		if (Enemy->GetCurrentWeapon() == Enemy->Weapon.SwordWeapon && Enemy->Weapon.SwordWeapon->CanParry)
		{
			IsParry++;
			FTimerHandle myTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					IsParry--;
					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);// 타이머 초기화
				}), 1.0f, false);
			if (Enemy->ACAnim->GetCurrentActiveMontage() != nullptr)
			{
				UAnimMontage* CurrentMontage = Enemy->ACAnim->GetCurrentActiveMontage();
				EnemyAttackSectionIndex = CurrentMontage->GetSectionIndex(Enemy->ACAnim->Montage_GetCurrentSection(CurrentMontage));
				
			}
			break;
		}
	}

}

void ASword::PlayParry()
{
	Character->ACAnim->PlaySwordParryMontage(EnemyAttackSectionIndex);
}


