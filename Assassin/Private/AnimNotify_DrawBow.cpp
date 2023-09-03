// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_DrawBow.h"

#include "Weapons/Bow.h"
#include "Character/AssassinCharacter.h"
#include "Weapons/Arrow.h"
#include "Weapons/BowAnimInstance.h"

void UAnimNotify_DrawBow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AAssassinCharacter* Character = Cast<AAssassinCharacter>(MeshComp->GetOwner());
	if(Character == nullptr || Character->Weapon.BowWeapon == nullptr && Character->Weapon.BowWeapon != Character->GetCurrentWeapon()) return;

	//손에 붙이기 위해 IsDrawing 설정
	Character->Weapon.BowWeapon->BowAnim->IsDrawing = IsStart;

	
	if(Character->Weapon.BowWeapon->CurrentArrow == nullptr) return;

	if(IsStart)
	{
		/*
		Character->Weapon.BowWeapon->CurrentArrow->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Character->Weapon.BowWeapon->CurrentArrow->SetActorLocation(FVector::ZeroVector);
		Character->Weapon.BowWeapon->CurrentArrow->SetActorRotation(FRotator::ZeroRotator);
		Character->Weapon.BowWeapon->CurrentArrow->AttachToComponent(Character->Weapon.BowWeapon->GetSkeletalMesh(),FAttachmentTransformRules::KeepRelativeTransform,"Arrow_Socket");
		*/
	}
	else
	{
		Character->Weapon.BowWeapon->CurrentArrow->Attack();
	}
	
}
