// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSlot.h"

#include "Component/RangeComponent.h"
#include "Components/Image.h"

void UWeaponSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UWeaponSlot::ChangeImage(ERangeWeaponState ChangeState)
{
	switch (ChangeState)
	{
	case ERangeWeaponState::E_Throwdagger:
		WeaponImg->SetBrushFromTexture(ThrowDaggerImg,true);
		break;
	case ERangeWeaponState::E_SmokeBomb:
		WeaponImg->SetBrushFromTexture(SmokeImg,true);
		break;
	}
}
