// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Bow.h"

#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "Weapons/BowAnimInstance.h"

ABow::ABow()
{
	BowSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKELETALMESHCOMPONENT"));
	RootComponent = BowSkeletalMeshComponent;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Script/Engine.SkeletalMesh'/Game/MedievalArmour/weapon/Props/Meshes/Bow.Bow'"));
	if (SK_WEAPON.Succeeded())
	{
		BowSkeletalMeshComponent->SetSkeletalMesh(SK_WEAPON.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> ANIM_BP(TEXT("/Script/Engine.AnimBlueprint'/Game/AMainDirectory/Weapon/Animation/BP_BowAnim.BP_BowAnim'"));
	if (ANIM_BP.Succeeded())
	{
		BowSkeletalMeshComponent->SetAnimInstanceClass(ANIM_BP.Object->GeneratedClass);
	}
	

}

void ABow::BeginPlay()
{
	Super::BeginPlay();
	BowSkeletalMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	BowAnim = Cast<UBowAnimInstance>(BowSkeletalMeshComponent->GetAnimInstance());
}

void ABow::Attack()
{
	IsAttacking = true;
	Character->ACAnim->PlayArrowShootMontage();
}

void ABow::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	BowAnim->StringTransform = Character->GetMesh()->GetSocketLocation("ArrowSocket");
}

void ABow::InitializeWeapon(AAssassinCharacter* OwnerCharacter)
{
	Super::InitializeWeapon(OwnerCharacter);
	Character->ACAnim->OnMontageEnded.AddDynamic(this, &ABow::OnAttackMontageEnded);
}

void ABow::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName().Contains("ShootArrow"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Bow: IsAttacking End---"));
		IsAttacking = false;
	}
}
