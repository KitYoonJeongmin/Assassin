// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/RangeComponent.h"

#include "Camera/CameraComponent.h"
#include "Character/AssassinCharacter.h"
#include "Component/SmokeBombComponent.h"
#include "Component/ThrowDaggerComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
URangeComponent::URangeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ThrowDaggerComponent = CreateDefaultSubobject<UThrowDaggerComponent>(TEXT("THROWDAGGERCOMPONENT"));
	SmokeBombComponent = CreateDefaultSubobject<USmokeBombComponent>(TEXT("SMOKEBOMBCOMPNENT"));
	
	EndLocation = ZoomOutLocation;
}


// Called when the game starts
void URangeComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentProjectileComp = ThrowDaggerComponent;
	CurrentRangeWeaponState = ERangeWeaponState::E_Throwdagger;
	
	OwnerCharacter = Cast<AAssassinCharacter>(GetOwner());
	ThrowDaggerComponent->SetOwnerCharacter(OwnerCharacter);
	SmokeBombComponent->SetOwnerCharacter(OwnerCharacter);

	ZoomOutFOV = OwnerCharacter->FollowCamera->FieldOfView;
	CurrentFOV = ZoomOutFOV;
	TargetFOV = ZoomOutFOV;
}


// Called every frame
void URangeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector StartLocation = OwnerCharacter->CameraBoom->SocketOffset;
	float Alpha = 1.f;
	
	OwnerCharacter->CameraBoom->SocketOffset = FMath::VInterpTo(StartLocation, EndLocation,DeltaTime, Alpha);
	//OwnerCharacter->CameraBoom->SocketOffset = EndLocation;

	float FOVAlpha = 0.1f; // 보간 비율 (0.0f ~ 1.0f)

	CurrentFOV = FMath::Lerp(CurrentFOV, TargetFOV, FOVAlpha);
	OwnerCharacter->FollowCamera->SetFieldOfView(CurrentFOV);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("EndLocation: %f"), FOV));
}

void URangeComponent::ZoomIn()
{

	EndLocation = ZoomInLocation;
	TargetFOV = ZoomInFOV;
	CurrentProjectileComp->DrawGuide();
}

void URangeComponent::ZoomOut()
{
	EndLocation = ZoomOutLocation;
	TargetFOV = ZoomOutFOV;
	CurrentProjectileComp->DeleteGuide();
}

void URangeComponent::Shoot()
{
	CurrentProjectileComp->ShootProjectile();
}

void URangeComponent::ChangeWeapon(const FInputActionValue& Value)
{
	float Input = Value.Get<float>();
	UEnum* Enum = StaticEnum<ERangeWeaponState>();
	uint8 CurrentRangeWeaponStateNum = (uint8)CurrentRangeWeaponState;
	UE_LOG(LogTemp,Warning,TEXT("Current weapon:%d"),CurrentRangeWeaponStateNum);
	if(Input==1)
	{
		if(CurrentRangeWeaponStateNum<Enum->NumEnums()-1)
		{
			++CurrentRangeWeaponStateNum;
			SetCurrentRangeWeaponState((ERangeWeaponState)Enum->GetValueByIndex(CurrentRangeWeaponStateNum));
		}
	}
	else
	{
		if(CurrentRangeWeaponStateNum>0)
		{
			--CurrentRangeWeaponStateNum;
			SetCurrentRangeWeaponState((ERangeWeaponState)Enum->GetValueByIndex(CurrentRangeWeaponStateNum));
		}
	}
	
}

void URangeComponent::SetCurrentRangeWeaponState(ERangeWeaponState ChangeState)
{
	switch (ChangeState)
	{
	case ERangeWeaponState::E_Throwdagger:
		CurrentProjectileComp = ThrowDaggerComponent;
		break;
	case ERangeWeaponState::E_SmokeBomb:
		CurrentProjectileComp = SmokeBombComponent;
		break;
	}
	CurrentRangeWeaponState = ChangeState;
	OnChangeRangeWeaponState.Broadcast(CurrentRangeWeaponState);
}


