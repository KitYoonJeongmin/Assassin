// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_BossLaunchSkill.h"

#include "Character/ACAnimInstance.h"
#include "Character/Enemy/Boss.h"

void UAnimNotify_BossLaunchSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABoss* Boss = Cast<ABoss>(MeshComp->GetOwner());
	if(Boss)
	{
		Boss->LaunchSkill();
	}
}
