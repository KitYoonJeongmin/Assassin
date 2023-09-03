// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnArrow.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/AssassinCharacter.h"
#include "Character/Enemy/AIControllerBase.h"
#include "Weapons/Arrow.h"
#include "Weapons/Bow.h"

void UAnimNotify_SpawnArrow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AAssassinCharacter* Character = Cast<AAssassinCharacter>(MeshComp->GetOwner());
	if(Character && Character->Weapon.BowWeapon && Character->Weapon.BowWeapon == Character->GetCurrentWeapon())
	{
		//FVector ArrowLoc = Character->GetMesh()->GetBoneLocation("hand_r");
		
		//생성
		Character->Weapon.BowWeapon->CurrentArrow = GetWorld()->SpawnActor<AArrow>(FVector::ZeroVector, FRotator::ZeroRotator); 
		//손에 붙여줌
		Character->Weapon.BowWeapon->CurrentArrow->AttachToComponent(Character->GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,"ArrowSocket");
		AAIControllerBase* CharacterController = Cast<AAIControllerBase>(Character->GetController());
		if(CharacterController)
		{
			AAssassinCharacter* Target = Cast<AAssassinCharacter>(CharacterController->GetBlackboardComponent()->GetValueAsObject(AAIControllerBase::TargetKey));
			if(Target) Character->Weapon.BowWeapon->CurrentArrow->TargetLoc = Target->GetActorLocation();
		}
	}
	
}
