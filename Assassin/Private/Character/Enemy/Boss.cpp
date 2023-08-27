// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Boss.h"

#include "Character/ACAnimInstance.h"
#include "Character/Enemy/BossAIController.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapons/BossExplosionSkill.h"

ABoss::ABoss()
{
	
	AIControllerClass = ABossAIController::StaticClass();
	HealthPoint = 500.f;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoss::LaunchSkill()
{
	FVector StartLoc = GetActorLocation();
	FVector EndLoc = StartLoc;
	float Radius = 500.f;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<FHitResult> HitResults;
	
	if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLoc, EndLoc, Radius, DetectEnemyTrace, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		// 주변 적 감지
		TArray<AAssassinCharacter*> Enemys;
		for(auto HitResult : HitResults)
		{
			AAssassinCharacter* CurrentEnemy = Cast<AAssassinCharacter>(HitResult.GetActor());
			if(CurrentEnemy)
			{
				Enemys.AddUnique(CurrentEnemy);
			}
		}

		// 뒤로 밀어주기
		for(auto Enemy:Enemys)
		{
			FVector LaunchVec = Enemy->GetActorLocation() - GetActorLocation();
			LaunchVec.Z = 0.f;
			LaunchVec.Normalize();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("LOCATION: %s"), *LaunchVec.ToString()));
			Enemy->ACAnim->StopAllMontages(0.0f);
			//Enemy->OnStunStart.Broadcast();
			Enemy->ACAnim->PlaySwordHitMontage();
			Enemy->LaunchCharacter(Enemy->GetActorUpVector()*50.f,false,true);
			Enemy->LaunchCharacter(LaunchVec*1200.f,true,false);
		}
	}
}

void ABoss::ExplosionSkill(FVector TargetVec)
{
	ABossExplosionSkill* SpawnedSkill = GetWorld()->SpawnActor<ABossExplosionSkill>(BossExplosionSkillClass, TargetVec, FRotator(0.f,0.f,0.f));
	//SpawnedSkill->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	//SpawnedSkill->InitializeWeapon();
	
}
