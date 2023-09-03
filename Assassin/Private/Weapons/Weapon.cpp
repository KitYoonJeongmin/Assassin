// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"
#include "Character/AssassinCharacter.h"
#include "Character/ACAnimInstance.h"
#include "Component/FinisherComponent.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	FinisherComp = CreateDefaultSubobject<UFinisherComponent>(TEXT("FINISHER"));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	IsAttacking = false;
	IsUsed = false;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!IsUsed) return;

}

void AWeapon::InitializeWeapon(AAssassinCharacter* OwnerCharacter)
{
	Character = Cast<AAssassinCharacter>(OwnerCharacter);
}
void AWeapon::Detach()
{
	UPrimitiveComponent* WeaponPrimitive = Cast<UPrimitiveComponent>(RootComponent);
	if (WeaponPrimitive != nullptr)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		WeaponPrimitive->SetCollisionProfileName("DetachWeapon");
		WeaponPrimitive->SetSimulatePhysics(true);
		WeaponPrimitive->SetEnableGravity(true);
	}
	
}
bool AWeapon::GetIsAttacking() { return IsAttacking; }
UFinisherComponent* AWeapon::GetFinisherComponent() { return FinisherComp; }