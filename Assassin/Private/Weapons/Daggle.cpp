// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Daggle.h"

#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Character/ACAnimInstance.h"
#include "Character/Enemy/Enemy.h"

#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ADaggle::ADaggle()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Script/Engine.StaticMesh'/Game/MedievalArmour/CharacterParts/Meshes/StaticMeshes/SM_wp_sword_04.SM_wp_sword_04'"));
	if (SM_WEAPON.Succeeded())
	{
		Mesh->SetStaticMesh(SM_WEAPON.Object);
	}
}

void ADaggle::InitializeWeapon()
{
	Super::InitializeWeapon();
	Character->ACAnim->OnMontageEnded.AddDynamic(this,&ADaggle::OnAttackMontageEnded);
}

void ADaggle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!IsUsed)
	{
		if(TargetCharacter)
		{
			TargetCharacter->SetCustomDepth(false);
			TargetCharacter = nullptr;
		}
		return;
	}
	FindTargetCharacter();
}

void ADaggle::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage->GetName().Contains("Stealth"))
	{
		Character->GetCapsuleComponent()->SetCollisionProfileName("PlayerCollision");
	}
}



void ADaggle::Attack()
{
	if(TargetCharacter)
	{
		TargetCharacter->SetActorRotation(Character->GetActorRotation());
		
		FVector TargetLoc = TargetCharacter->GetActorLocation() - TargetCharacter->GetActorForwardVector()*137.f;
		FRotator TargetRot = TargetCharacter->GetActorRotation();
		Character->MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("AssassinTarget"), TargetLoc, TargetRot);
		Character->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
		TargetCharacter->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
		UAnimMontage* AssassinMon = Character->ACAnim->PlayAssassinMontage();

		
		TargetCharacter->ACAnim->PlayAssassinedMontage(AssassinMon);
	}
}

void ADaggle::FindTargetCharacter()
{
	if(Character == nullptr) return;
	
	float Radius = 200.f;
	float Distance = 300.f;

	FRotator TraceForwardRot = Character->GetController()->GetDesiredRotation();
	TraceForwardRot.Pitch +=20.f;
	FVector TarceForwardVec = TraceForwardRot.Vector();
	//Find Nearest Enemy
	FVector StartLoc = Character->GetActorLocation() + TarceForwardVec*Radius;
	FVector EndLoc = StartLoc + TarceForwardVec*Distance;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	TArray<FHitResult> HitResults;
	bool CanAttack = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLoc, EndLoc, Radius, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);


	AAssassinCharacter* NewTarget = nullptr;
	if(CanAttack)
	{
		for(auto HitResult : HitResults)
		{
			AAssassinCharacter* Target = Cast<AAssassinCharacter>(HitResult.GetActor());
			FHitResult HitResult2;
			if(Target)
			{
				//적사이 물체가 없는지 확인
				TArray<AActor*> ActorsToIgnore2;
				ActorsToIgnore2.Add(Character);
				ActorsToIgnore2.Add(Target);
				if(!UKismetSystemLibrary::LineTraceSingle(GetWorld(),Character->GetActorLocation(),Target->GetActorLocation(),UEngineTypes::ConvertToTraceType(ECC_Visibility),false,ActorsToIgnore2, EDrawDebugTrace::None, HitResult2, true))
				{
					//가장 가까운 적 구하기
					if(NewTarget)
					{
						if(Character->GetDistanceTo(Target) < Character->GetDistanceTo(NewTarget))
						{
							NewTarget = Target;
						}
					}
					else
					{
						NewTarget = Target;
					}
				}
			}
		}
	}

	//갱싱
	if (TargetCharacter != NewTarget)
	{
		if(NewTarget)
		{
			NewTarget->SetCustomDepth(true);
		}
		if(TargetCharacter)
		{
			TargetCharacter->SetCustomDepth(false);
		}
	}
	TargetCharacter = NewTarget;
}