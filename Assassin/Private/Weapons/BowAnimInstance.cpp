// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BowAnimInstance.h"

void UBowAnimInstance::PlayDrawBowMontage()
{
	Montage_Play(DrawBow, 1.0f);
}
