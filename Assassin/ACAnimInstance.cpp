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

	//Foot IK
	FootRotL = FootIKComp->m_pFootRotation_Left;
	FootRotR = FootIKComp->m_pFootRotation_Right;
	HipOffset = FootIKComp->m_fHipOffset;
	FootOffsetL = FootIKComp->m_fFootOffsetLeft;
	FootOffsetR = FootIKComp->m_fFootOffsetRight;

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
