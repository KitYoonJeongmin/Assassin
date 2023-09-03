// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Dagger.h"

#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Character/ACAnimInstance.h"
#include "Character/Enemy/Enemy.h"

#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/CoverMovementComponent.h"
#include "Component/OutlineComponent.h"
#include "Components/CapsuleComponent.h"


#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ADagger::ADagger()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Script/Engine.StaticMesh'/Game/MedievalArmour/CharacterParts/Meshes/StaticMeshes/SM_wp_sword_04.SM_wp_sword_04'"));
	if (SM_WEAPON.Succeeded())
	{
		Mesh->SetStaticMesh(SM_WEAPON.Object);
	}
	CanAttackForward = false;
}

void ADagger::InitializeWeapon(AAssassinCharacter* OwnerCharacter)
{
	Super::InitializeWeapon(OwnerCharacter);
	Character->ACAnim->OnMontageEnded.AddDynamic(this,&ADagger::OnAttackMontageEnded);
}

void ADagger::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!IsUsed)
	{
		if(TargetCharacter)
		{
			//TargetCharacter->SetCustomDepth(false);
			TargetCharacter->OutlineComponent->DisableOverlay();
			TargetCharacter = nullptr;
		}
		return;
	}
	//아래에 있는 적을 찾을지 전방의 적을 찾을지 검사
	CanAttackForward = FindCanAttackForward();
	if(CanAttackForward)
	{
		FindTargetCharacter();
	}
	else
	{
		FindUnderTargetCharacter();
	}
	
}

void ADagger::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage->GetName().Contains("Stealth"))
	{
		Character->GetCapsuleComponent()->SetCollisionProfileName("PlayerCollision");
	}
}



void ADagger::Attack()
{
	if(TargetCharacter)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("Attack!"));
		if(CanAttackForward)
		{
			ForwardAttack();
		}
		else
		{
			UnderAttack();
		}
	}
	
}

void ADagger::ForwardAttack()
{
	FVector TargetForward = TargetCharacter->GetActorForwardVector();
	FVector CharacterForward = TargetCharacter->GetActorLocation()-Character->GetActorLocation();
	CharacterForward.Normalize();

	
	if(FVector::DotProduct(TargetForward, CharacterForward)<0)
	{
		FRotator TargetCharacterRot = Character->GetActorRotation();
		TargetCharacterRot.Yaw +=180.f;
		
		FVector TargetLoc = TargetCharacter->GetActorLocation() + TargetCharacter->GetActorForwardVector()*115.f;
		FRotator TargetRot = TargetCharacter->GetActorRotation();
		TargetRot.Yaw+=180.f;
		Character->MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("AssassinTarget"), TargetLoc, TargetRot);
		Character->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
		TargetCharacter->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
		UAnimMontage* AssassinMon = Character->ACAnim->PlayAssassinForwardMontage();
		
		TargetCharacter->ACAnim->PlayAssassinedForwardMontage(AssassinMon);
	}
	else
	{
		FVector TargetLoc = TargetCharacter->GetActorLocation() - TargetCharacter->GetActorForwardVector()*125.f;//+ TargetCharacter->GetActorRightVector()*10.f;
		FRotator TargetRot = TargetCharacter->GetActorRotation();
		Character->MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("AssassinTarget"), TargetLoc, TargetRot);
		Character->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
		TargetCharacter->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
		UAnimMontage* AssassinMon = Character->ACAnim->PlayAssassinMontage();
		
		TargetCharacter->ACAnim->PlayAssassinedMontage(AssassinMon);
	}
	
}

void ADagger::UnderAttack()
{
	TargetCharacter->SetActorRotation(Character->GetActorRotation());
		
	FVector TargetLoc = TargetCharacter->GetActorLocation() - TargetCharacter->GetActorForwardVector()*60.f;
	FRotator TargetRot = TargetCharacter->GetActorRotation();
	Character->MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("AssassinTarget"), TargetLoc, TargetRot);
	Character->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
	TargetCharacter->GetCapsuleComponent()->SetCollisionProfileName("CharacterMesh");
	Character->ACAnim->PlayUnderAssassinationMontage(TargetCharacter);
	
}

void ADagger::FindTargetCharacter()
{
	//전방에 있는 적을 찾음
	if(Character == nullptr) return;
	
	float Radius = 200.f;
	float Distance = 200.f;

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
				if(!UKismetSystemLibrary::LineTraceSingle(GetWorld(),Character->GetActorLocation(),Target->GetActorLocation(),UEngineTypes::ConvertToTraceType(ECC_WorldStatic),false,ActorsToIgnore2, EDrawDebugTrace::None, HitResult2, true))
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
			//NewTarget->SetCustomDepth(true);
			NewTarget->OutlineComponent->EnableOverlay();
		}
		if(TargetCharacter)
		{
			//TargetCharacter->SetCustomDepth(false);
			TargetCharacter->OutlineComponent->DisableOverlay();
		}
	}
	TargetCharacter = NewTarget;
}

void ADagger::FindUnderTargetCharacter()
{
	if(Character == nullptr) return;
	
	float Radius = 200.f;
	float Distance = 500.f;

	FRotator TraceForwardRot = Character->GetController()->GetDesiredRotation();
	TraceForwardRot.Pitch +=10.f;
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
				if(!UKismetSystemLibrary::LineTraceSingle(GetWorld(),Character->GetActorLocation(),Target->GetActorLocation(),UEngineTypes::ConvertToTraceType(ECC_WorldStatic),false,ActorsToIgnore2, EDrawDebugTrace::None, HitResult2, true))
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
			//NewTarget->SetCustomDepth(true);
			NewTarget->OutlineComponent->EnableOverlay();
		}
		if(TargetCharacter)
		{
			//TargetCharacter->SetCustomDepth(false);
			TargetCharacter->OutlineComponent->DisableOverlay();
		}
	}
	TargetCharacter = NewTarget;
}

bool ADagger::FindCanAttackForward()
{
	float Radius = 40.f;
	FRotator TraceForwardRot = Character->GetController()->GetDesiredRotation();
	TraceForwardRot.Pitch = 0.f;//Character->GetActorLocation().Z - 90.f - Radius;
	FVector TarceForwardVec = TraceForwardRot.Vector();
	TarceForwardVec.Normalize();
	
	FVector StartLoc = Character->GetActorLocation() + TarceForwardVec*180.f - Character->GetActorUpVector()*(Radius+20.f);
	FVector EndLoc = StartLoc - Character->GetActorUpVector()*50.f;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	FHitResult HitResults;
	bool IsValidFloor = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLoc, EndLoc, Radius, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);
	return IsValidFloor;
}