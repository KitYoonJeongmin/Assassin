// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BossExplosionSkill.h"

#include "NiagaraComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ABossExplosionSkill::ABossExplosionSkill()
{
	ExplosionNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NIAGARACOMPONENT"));
	ExplosionNiagara->SetRelativeScale3D(FVector(5.f,5.f,5.f));
	ExplosionNiagara->SetupAttachment(RootComponent);
	//변수 초기화
	WeaponDamage = 50.f;
	ExplosionRange = 300.f;
	ExplosionDelayTime = 4.f;
}

void ABossExplosionSkill::BeginPlay()
{
	Super::BeginPlay();
	ExplosionNiagara->Deactivate();
	
	auto RangeDecal = UGameplayStatics::SpawnDecalAtLocation(this, RangeDecalMaterial, FVector(ExplosionRange,ExplosionRange,ExplosionRange), GetActorLocation(), FRotator(-90.f,0.f,0.f) , ExplosionDelayTime);
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &ABossExplosionSkill::Attack, ExplosionDelayTime, false);
}

void ABossExplosionSkill::Attack()
{
	ExplosionNiagara->SetWorldLocation(GetActorLocation());
	ExplosionNiagara->Activate();
	FVector StartLoc = GetActorLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<FHitResult> HitResults;
	
	if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLoc, StartLoc, ExplosionRange, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		TArray<APlayerCharacter*> Enemys;
		for(auto HitResult:HitResults)
		{
			APlayerCharacter* Enemy = Cast<APlayerCharacter>(HitResult.GetActor());
			if(Enemy&&!Enemys.Contains(Enemy))
			{
				Enemy->TakeDamage(WeaponDamage,DamageEvent,Character ? Character->GetController() : nullptr,this);
				Enemys.AddUnique(Enemy);
			}
		}
	}
}
