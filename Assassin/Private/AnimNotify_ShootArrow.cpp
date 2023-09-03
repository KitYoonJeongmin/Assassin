// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ShootArrow.h"

#include "Character/AssassinCharacter.h"
#include "Weapons/Arrow.h"
#include "Weapons/Bow.h"

void UAnimNotify_ShootArrow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AAssassinCharacter* Character = Cast<AAssassinCharacter>(MeshComp->GetOwner());
	if(Character == nullptr) return;
	ABow* CurrentBow = Cast<ABow>(Character->GetCurrentWeapon());

	if(CurrentBow)
	{
		CurrentBow->CurrentArrow->Attack();
	}
}
