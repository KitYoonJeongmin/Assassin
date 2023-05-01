// Fill out your copyright notice in the Description page of Project Settings.


#include "ACAnimInstance.h"
#include "AssassinCharacter.h"
#include "FootIKComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UACAnimInstance::UACAnimInstance()
{
	FootIKComp = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIK"));

}

void UACAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	FootIKComp->m_pCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UACAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	Character = Cast<AAssassinCharacter>(TryGetPawnOwner());

	if (!::IsValid(Character)) { return; }

	//Speed
	CurrentPawnSpeed = Character->GetVelocity().Size();
	//Jump
	IsFalling = Character->GetMovementComponent()->IsFalling();

	//IK
	FootRotL = FootIKComp->m_pFootRotation_Left;
	FootRotR = FootIKComp->m_pFootRotation_Right;
	HipOffset = FootIKComp->m_fHipOffset;
	FootOffsetL = FootIKComp->m_fFootOffsetLeft;
	FootOffsetR = FootIKComp->m_fFootOffsetRight;
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
	TArray<USceneComponent*> ChildrenMesh;
	Character->GetMesh()->GetChildrenComponents(true, ChildrenMesh);

	if (ChildrenMesh.Num() == 0) return;
	for (auto SKMesh : ChildrenMesh)
	{

		if (Cast<USkeletalMeshComponent>(SKMesh) != nullptr)
		{
			if (Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance()) != nullptr)
				Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance())->Montage_Play(ClimbUpMon, 1.0f);
		}

	}
	Montage_Play(ClimbUpMon, 1.0f);
}

void UACAnimInstance::UpdateMovementState(EMovementState CurrentState)
{
	CurrnetMovementState = CurrentState;
}

void UACAnimInstance::SetCanMoveLedge(bool Left, bool Right)
{
	TArray<USceneComponent*> ChildrenMesh;
	Character->GetMesh()->GetChildrenComponents(true, ChildrenMesh);

	if (ChildrenMesh.Num() == 0) return;
	for (auto SKMesh : ChildrenMesh)
	{

		if (Cast<USkeletalMeshComponent>(SKMesh) != nullptr)
		{
			if (Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance()) != nullptr)
			{
				Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance())->CanMoveLeftOnLedge = Left;
				Cast<UACAnimInstance>(Cast<USkeletalMeshComponent>(SKMesh)->GetAnimInstance())->CanMoveRightOnLedge = Right;
			}
				
		}
	}
	CanMoveLeftOnLedge = Left;
	CanMoveRightOnLedge = Right;
}

void UACAnimInstance::AnimNotify_ClimbJump()
{
	//UE_LOG(LogTemp, Log, TEXT("Climb"));
	UpdateMovementState(Character->CurrnetMovementState);
	Character->Jump();
	Character->ClimbingComp->GrabLedge();
}

void UACAnimInstance::AnimNotify_ClimbUpEnd()
{
	/*
	Character->UpdateMovementState(EMovementState::E_Walking);
	Cast<UCharacterMovementComponent>(Character->GetMovementComponent())->SetMovementMode(MOVE_Walking);
	Cast<UCharacterMovementComponent>(Character->GetMovementComponent())->bOrientRotationToMovement = true;
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	Character->ClimbingComp->ClimbingState.CanClimbOnLedge = false;
	Character->ClimbingComp->ClimbingState.ClimbUpLedge = false;
	Character->ClimbingComp->ClimbingState.DropDown = false;
	Character->ClimbingComp->ClimbingState.HighMantle = false;
	*/
	Character->ClimbingComp->Fall();
}
