// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SmokeBomb.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Enemy/MeleeAIController.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ASmokeBomb::ASmokeBomb()
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SmokeBombNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/AMainDirectory/SparkFX/NS_Smoke.NS_Smoke'"));
	
	NiagaraComponent->SetAsset(SmokeBombNiagara.Object);
	NiagaraComponent->SetAutoActivate(false);

	SightBlockComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("SightBlockActorComp"));

	IsBomb = false;
}

void ASmokeBomb::BeginPlay()
{
	Super::BeginPlay();
	SightBlockComp->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	SightBlockComp->SetRelativeLocation(FVector(0.f,0.f,0.f));
	if(SightBlockComp->GetChildActor())
	{
		SightBlockComp->GetChildActor()->SetActorEnableCollision(false);
	}
}

void ASmokeBomb::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	StunEnemy();
}

void ASmokeBomb::ExcuteEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::ExcuteEffect(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	IsBomb = true;
	
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetHiddenInGame(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraComponent->GetAsset(), GetActorLocation(), GetActorRotation());

	if(SightBlockComp->GetChildActor())
	{
		SightBlockComp->GetChildActor()->SetActorEnableCollision(true);
	}
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		NiagaraComponent->Deactivate();
	}), 7.f, false);
	FTimerHandle WaitHandle2;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle2, FTimerDelegate::CreateLambda([&]()
	{
		for(auto Enemy : Enemys)
		{
			Cast<AMeleeAIController>(Enemy->GetController())->CanSense = true;
		}
		Destroy();

	}), 10.f, false);
}

void ASmokeBomb::Shoot(FVector TargetLoc)
{
	FVector Velocity = TargetLoc;
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetAllPhysicsLinearVelocity(Velocity);
}

void ASmokeBomb::StunEnemy()
{
	if(!IsBomb) return;

	FVector StartLoc = GetActorLocation();
	float SearchRadius = 400.f; 
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<FHitResult> HitResults;
	if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLoc, StartLoc, SearchRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		for(auto HitResult:HitResults)
		{
			AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
			if(Enemy&&!Enemys.Contains(Enemy))
			{
				Cast<AMeleeAIController>(Enemy->GetController())->CanSense = false;
				Cast<AMeleeAIController>(Enemy->GetController())->LoseSense();
				Enemy->OnStunStart.Broadcast();
				FTimerHandle myTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([Enemy]()
					{
						if(Enemy == nullptr) return;
						Enemy->OnStunStart.Broadcast();
					}), 5.f, false); 
				Enemys.AddUnique(Enemy);
			}
		}
	}
	
}
