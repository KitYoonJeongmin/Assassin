// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ProjectileBaseComponent.h"

#include "Character/PlayerCharacter.h"

// Sets default values for this component's properties
UProjectileBaseComponent::UProjectileBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsZooming(false);
}


// Called when the game starts
void UProjectileBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProjectileBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProjectileBaseComponent::SetIsZooming(bool ZoomState)
{
	IsZooming = ZoomState;
	if(IsZooming && Cast<APlayerCharacter>(OwnerCharacter)) Cast<APlayerCharacter>(OwnerCharacter)->CurrentPlayerWeaponState = EPlayerWeaponState::E_Range;
	else if(!IsZooming && Cast<APlayerCharacter>(OwnerCharacter)) Cast<APlayerCharacter>(OwnerCharacter)->CurrentPlayerWeaponState = EPlayerWeaponState::E_Melee;
}


