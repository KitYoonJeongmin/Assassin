// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileBase.h"

#include "Character/AssassinCharacter.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SPHERECOMPONENT"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("STATICMESHCOMPNENT"));
	TrailParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticleComp"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> TRAILPARTICLE(TEXT("/Script/Engine.ParticleSystem'/Game/AMainDirectory/Weapon/Trail/P_ProjectileTrail.P_ProjectileTrail'"));
	TrailParticleSystemComponent->SetTemplate(TRAILPARTICLE.Object);
	
	
	RootComponent = StaticMeshComponent;
	SphereComponent->SetupAttachment(RootComponent);
	TrailParticleSystemComponent->AttachToComponent(StaticMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AAssassinCharacter>(GetOwner());
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::ExcuteEffect);
	TrailParticleSystemComponent->BeginTrails(FName("Trail1"), FName("Trail2"), ETrailWidthMode::ETrailWidthMode_FromFirst, 3);
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::ExcuteEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Hit Actor %s"), *(SweepResult.GetActor()->GetName())));
	
}

