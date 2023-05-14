// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "Weapons/Sword.h"
#include "Component/FootIKComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UACAnimInstance::UACAnimInstance()
{
	
	WeaponState = EWeaponState::E_Daggle;
}

void UACAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = Cast<AAssassinCharacter>(TryGetPawnOwner());
	
}

void UACAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	

	if (!::IsValid(Character)) { return; }

	//Speed
	CurrentPawnSpeed = Character->GetVelocity().Size();
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

	//MoveTo Other Ledge
	CanMoveOtherLedge = Character->ClimbingComp->ClimbingState.CanMoveOnLedge;
	InputRightAxis = Character->MovementVector.X;
	InputForwardAxis = Character->MovementVector.Y;
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
	Montage_Play(SwordEquipMon, 1.2f);
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
	Montage_Play(SwordAttackMon, 1.0f);
}

void UACAnimInstance::PlaySwordHitMontage(FVector AttackVector)
{
	FName SectionName;
	//방향 구하기
	float DotProduct = FVector::DotProduct(AttackVector, Character->GetMesh()->GetRightVector());
	if (DotProduct >= 0.5)	//왼쪽
	{
		//UE_LOG(LogTemp, Warning, TEXT("Left!"));
		SectionName = FName("Left");
	}
	else if (DotProduct <= -0.5) //오른쪽
	{
		//UE_LOG(LogTemp, Warning, TEXT("Right!"));
		SectionName = FName("Right");
	}
	else
	{
		float DotForward = FVector::DotProduct(AttackVector, Character->GetMesh()->GetForwardVector());
		if (DotForward >0)	//뒤
		{
			//UE_LOG(LogTemp, Warning, TEXT("Back!"));
			SectionName = FName("Back");
		}
		else if (DotForward < 0) //앞
		{
			//UE_LOG(LogTemp, Warning, TEXT("Forward!"));
			SectionName = FName("Forward");
		}
	}
	//몽타주 실행
	Montage_Play(SwordHitMon, 1.0f);
	Montage_JumpToSection(SectionName, SwordHitMon);
}

void UACAnimInstance::UpdateMovementState(EMovementState CurrentState)
{
	CurrnetMovementState = CurrentState;
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
	//단검은 다른곳에 붙이기
	Character->AttachWeaponTo(Character->Weapon.SwordWeapon, FName("Sword_rSocket"), true);
	
}

void UACAnimInstance::AnimNotify_UnEquipment()
{
	
	Character->AttachWeaponTo(Character->Weapon.SwordWeapon, FName("SwordSocket"), false);
	//Character->AttachWeaponTo(Character->Weapon., FName(""), true); //단검을 손에
}
