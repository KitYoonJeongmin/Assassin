// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ThrowDagger.h"

#include "Character/Enemy/Enemy.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"

AThrowDagger::AThrowDagger()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Script/Engine.StaticMesh'/Game/MedievalArmour/CharacterParts/Meshes/StaticMeshes/SM_wp_sword_04.SM_wp_sword_04'"));
	if (SM_WEAPON.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(SM_WEAPON.Object);
	}
}

void AThrowDagger::BeginPlay()
{
	Super::BeginPlay();
	
	FRotator newRot = GetActorRotation();
	newRot.Yaw += 90.f;
	SetActorRotation(newRot);
	
}

void AThrowDagger::ExcuteEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::ExcuteEffect(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,SweepResult);

	AEnemy* HitEnemy = Cast<AEnemy>(SweepResult.GetActor());
	if(HitEnemy)
	{
		FDamageEvent DamageEvent;
		
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("BoneName: %s"), *(SweepResult.GetComponent()->GetName())));
		if(SweepResult.GetComponent()->GetName().Contains("head"))
		{
			HitEnemy->TakeDamage(HeadDamamge,DamageEvent,OwnerCharacter->GetController(), this);
		}
		else
		{
			HitEnemy->TakeDamage(BodyDamage,DamageEvent,OwnerCharacter->GetController(), this);
		}
	}
	Destroy();
}

void AThrowDagger::Shoot(FVector TargetLoc)
{
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(RootComponent, TargetLoc, GetActorRotation(),true,true,0.4f* FVector::Distance(GetActorLocation(),TargetLoc)/4000.f, false,EMoveComponentAction::Type::Move,Info);
}
