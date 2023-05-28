﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Character/Enemy/Enemy.h"
#include "Weapons/Sword.h"
#include "Weapons/Daggle.h"
#include "Component/FinisherComponent.h"
#include "Component/FootIKComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Containers/Map.h"


UACAnimInstance::UACAnimInstance()
{
	WeaponState = EWeaponState::E_Daggle;
	IsBlock = false;

	//HeadTracking
	HeadTrackingRadius = 300.f;
	CanHeadTracking = true;
}

void UACAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = Cast<AAssassinCharacter>(TryGetPawnOwner());
	//Montage Priority
	for (auto Montage : AssassinationMap)
	{
		MontagePriority.Add(Montage.Key);
		MontagePriority.Add(Montage.Value);
	}
	for (auto Montage : SwordFinisherMap)
	{
		MontagePriority.Add(Montage.Key);
		MontagePriority.Add(Montage.Value);
	}
	MontagePriority.Add(SwordHitMon);
	MontagePriority.Add(SwordParryMon);
	
	//HeadTracking
	if (Character->GetController())
	{
		HeadTrackingTargetLoc = Character->GetActorLocation() + Character->GetController()->GetControlRotation().Vector() * 300.f;
	}
}

void UACAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!::IsValid(Character)) { return; }

	//Speed
	CurrentPawnSpeed = Character->GetVelocity().Size();
	CurrentDirection = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());

	//Jump
	IsFalling = Character->GetMovementComponent()->IsFalling();

	//Foot IK
	FootRotL = Character->FootIKComp->m_pFootRotation_Left;
	FootRotR = Character->FootIKComp->m_pFootRotation_Right;
	HipOffset = Character->FootIKComp->m_fHipOffset;
	FootOffsetL = Character->FootIKComp->m_fFootOffsetLeft;
	FootOffsetR = Character->FootIKComp->m_fFootOffsetRight;

	//HandIK
	IKLeftHandLoc = Character->ClimbingComp->IKLeftHand;
	IKRightHandLoc = Character->ClimbingComp->IKRightHand;

	////MoveTo Other Ledge
	//CanMoveOtherLedge = Character->ClimbingComp->ClimbingState.CanMoveOnLedge;
	//InputRightAxis = Character->MovementVector.X;
	//InputForwardAxis = Character->MovementVector.Y;
	
	//HeadTracking
	TrackHead();
}


void UACAnimInstance::PlayClimbingIdleMon()
{
	TArray<USceneComponent*> ChildrenMesh;
	Character->GetMesh()->GetChildrenComponents(true, ChildrenMesh);

	if (ChildrenMesh.Num() == 0) return;
	for (auto SKMesh : ChildrenMesh)
	{

		if (Cast<USkeletalMeshComponent>(SKMesh) != nullptr)
		{
			if (Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance())!=nullptr)
				Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance())->Montage_Play(ClimbingIdleMon, 1.0f);
		}

	}
	Montage_Play(ClimbingIdleMon, 1.0f);
}

void UACAnimInstance::PlayClimbUpMon()
{
	Montage_Play(ClimbUpMon, 1.0f);
}

void UACAnimInstance::PlayEquipMontage(bool CanEquip)
{
	PlayMontagePriority(SwordEquipMon, 1.0f);
	if (CanEquip)
	{
		Montage_JumpToSection(FName("Equip"), SwordEquipMon);
	}
	else
	{
		Montage_JumpToSection(FName("Unequip"), SwordEquipMon);
	}
}

void UACAnimInstance::PlaySwordAttackMontage()
{
	if (!PlayMontagePriority(SwordAttackMon, 1.f))
	{
		if (Character->Weapon.SwordWeapon != nullptr)
		{
			Character->Weapon.SwordWeapon->AttackEndComboState();
		}
	}
}

void UACAnimInstance::PlaySwordHitMontage()
{
	Character->OnStunStart.Broadcast();
	if (IsBlock)
	{
		Montage_Play(SwordBlockHitMon, 1.f);
		return;
	}
	
	//몽타주 실행
	Montage_Play(SwordHitMon, 1.f);
}

void UACAnimInstance::PlaySwordRollMontage()
{
	Montage_Play(SwordRollMon, 1.f);
}

void UACAnimInstance::PlaySwordParryMontage(int32 MontageIndex)
{
	FName SelectionName = SwordParryMon->GetSectionName(MontageIndex);
	Montage_Play(SwordParryMon, 1.f);
	Montage_JumpToSection(SelectionName, SwordParryMon);
}

UAnimMontage* UACAnimInstance::PlayFinisherMontage()
{
	if (SwordFinisherMap.Contains(GetCurrentActiveMontage())) return nullptr;
	int32 MonIndex = FMath::RandRange(0, SwordFinisherMap.Num() - 1);
	class UAnimMontage* FinisherMon = SwordFinisherMap.begin().Key();
	for (auto FinishMonPair : SwordFinisherMap)
	{
		FinisherMon = FinishMonPair.Key;
		MonIndex--;
		if (MonIndex < 0)
		{
			break;
		}
	}
	if (FinisherMon != nullptr)
	{
		Montage_Play(FinisherMon, 1.f);
	}
	return FinisherMon;
}

void UACAnimInstance::PlayFinishedMontage(UAnimMontage* FinisherMon)
{
	
	UAnimMontage* FinishedMon = SwordFinisherMap.FindRef(FinisherMon);
	if (FinishedMon != nullptr)
	{
		OnFinished.Broadcast();
		Montage_Play(FinishedMon, 1.f);
	}
	
}
void UACAnimInstance::PlaySwordDeathMontage()
{
	Montage_Play(SwordDeathMon, 1.f);
}

UAnimMontage* UACAnimInstance::PlayAssassinMontage()
{
	int32 MonIndex = FMath::RandRange(0, AssassinationMap.Num() - 1);
	class UAnimMontage* AssassinMon = AssassinationMap.begin().Key();
	for (auto AssassinMonPair : AssassinationMap)
	{
		AssassinMon = AssassinMonPair.Key;
		MonIndex--;
		if (MonIndex < 0)
		{
			break;
		}
	}
	if(AssassinMon)
	{
		Montage_Play(AssassinMon, 1.f);
	}
	return AssassinMon;
}

void UACAnimInstance::PlayAssassinedMontage(UAnimMontage* FinisherMon)
{
	Montage_Play(AssassinationMap[FinisherMon], 1.f);
}

float UACAnimInstance::GetDistancebySwordFinish(UAnimMontage* FinisherMon)
{
	return SwordFinisherDistanceMap.FindRef(FinisherMon);
}

void UACAnimInstance::AnimNotify_Death()
{
	Character->Dead();
}

void UACAnimInstance::UpdateMovementState(EMovementState CurrentState)
{
	CurrnetMovementState = CurrentState;
}

void UACAnimInstance::MoveToLedge(bool IsStarted)
{
	//MoveTo Other Ledge
	CanMoveOtherLedge = IsStarted;
	if (CanMoveOtherLedge)
	{
		InputRightAxis = Character->MovementVector.X;
		InputForwardAxis = Character->MovementVector.Y;
	}
	else
	{
		InputRightAxis = 0;
		InputForwardAxis = 0;
	}
}

void UACAnimInstance::SetCanMoveLedge(bool Left, bool Right)
{
	CanMoveLeftOnLedge = Left;
	CanMoveRightOnLedge = Right;
}

void UACAnimInstance::AnimNotify_ClimbJump()
{
	//UE_LOG(LogTemp, Log, TEXT("Climb"));
	UpdateMovementState(Character->CurrnetMovementState);
	Character->Jump();
	Character->ClimbingComp->MoveToLedge();
}

void UACAnimInstance::AnimNotify_ClimbUpEnd()
{
	Character->ClimbingComp->Fall();
}

void UACAnimInstance::AnimNotify_MoveToLedge()
{
	Character->ClimbingComp->MoveToLedge();
}

void UACAnimInstance::AnimNotify_MoveToLedgeEnd()
{
	MoveToLedge(false);
	Character->ClimbingComp->ClimbingState.CanMoveOnLedge = false;
	Character->GetController()->EnableInput(Cast<APlayerController>(Character->GetController()));
	
}

FName UACAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UACAnimInstance::JumpToSwordAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), SwordAttackMon);
}

void UACAnimInstance::AnimNotify_CheckNextSection()
{
	OnNextAttackCheck.Broadcast();
}

void UACAnimInstance::AnimNotify_EnableAttackCheck()
{
	OnEnableAttackHitCheck.Broadcast();
}

void UACAnimInstance::AnimNotify_DisableAttackCheck()
{
	OnDisableAttackHitCheck.Broadcast();
}

void UACAnimInstance::AnimNotify_Equipment()
{
	Character->AttachWeaponTo(Character->Weapon.DaggleWeapon, FName("DaggleSocket"), false); //단검은 다른곳에 붙이기
	Character->AttachWeaponTo(Character->Weapon.SwordWeapon, FName("Sword_rSocket"), true);
	
}

void UACAnimInstance::AnimNotify_UnEquipment()
{
	
	Character->AttachWeaponTo(Character->Weapon.SwordWeapon, FName("SwordSocket"), false);
	Character->AttachWeaponTo(Character->Weapon.DaggleWeapon, FName("DaggleHandSocket"), true); //단검을 손에
}

void UACAnimInstance::AnimNotify_Camera()
{
	Character->GetCurrentWeapon()->GetFinisherComponent()->SwitchCamera(Cast<APlayerCharacter>(Character), true);
}

void UACAnimInstance::AnimNotify_CameraEnd()
{
	Character->GetCurrentWeapon()->GetFinisherComponent()->SwitchCamera(Cast<APlayerCharacter>(Character), false);
}


bool UACAnimInstance::PlayMontagePriority(UAnimMontage* PlayMontage, float PlayRate)
{
	UAnimMontage* CurrentMontage = GetCurrentActiveMontage();
	if (CurrentMontage != nullptr)
	{
		for (auto Montage : MontagePriority)
		{
			if (Montage == PlayMontage)	//실행하고자하는 몽타주의 우선순위가 더 높음
			{
				break;
			}
			else if(Montage == CurrentMontage)//실행되고 있는 몽타주의 우선순위가 더 높음
			{
				return false;
			}
		}
	}
	Montage_Play(PlayMontage, PlayRate);
	return true;
}

void UACAnimInstance::TrackHead()
{
	//HeadTracking
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Character);
	if (PlayerCharacter)
	{
		AEnemy* HeadTrackingEnemy = PlayerCharacter->FindNearestEnemy(PlayerCharacter->DetectNearByEnemy(HeadTrackingRadius));
		if (HeadTrackingEnemy)
		{
			FVector EnemyDirection = (HeadTrackingEnemy->GetActorLocation() - Character->GetActorLocation());
			EnemyDirection.Normalize();
			float DotProduct = FVector::DotProduct(EnemyDirection, Character->GetController()->GetControlRotation().Vector());
			float MaxDotProduct = FMath::Cos(FMath::DegreesToRadians(110.f));    //시야각 계산

			if (DotProduct > MaxDotProduct) //시야각 내에 있는지 확인
			{
				HeadTrackingTarget = HeadTrackingEnemy->GetMesh()->GetSocketLocation(FName("neck_01"));
			}
			else
			{
				HeadTrackingTarget = Character->GetActorLocation() + Character->GetController()->GetControlRotation().Vector() * 300.f;
			}
		}
		else
		{
			HeadTrackingTarget = Character->GetActorLocation() + Character->GetController()->GetControlRotation().Vector() * 300.f;
		}
		HeadTrackingTargetLoc = FMath::Lerp(HeadTrackingTarget, HeadTrackingTargetLoc, 0.95f);
		//DrawDebugSphere(GetWorld(), HeadTrackingTargetLoc, 30.f, 10, FColor(181, 0, 0));
	}
}