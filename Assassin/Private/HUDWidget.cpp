// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "WeaponSlot.h"
#include "Character/PlayerCharacter.h"
#include "Component/RangeComponent.h"
#include "Kismet/GameplayStatics.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
	PlayerCharacter->RangeComponent->OnChangeRangeWeaponState.AddDynamic(ThrowWeaponSlot,&UWeaponSlot::ChangeImage);
}
