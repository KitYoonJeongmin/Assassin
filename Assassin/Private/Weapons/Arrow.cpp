// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Arrow.h"

#include "Character/AssassinCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Bow.h"

AArrow::AArrow()
{
	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("PROJECTILECOMPONENT"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SPHERECOMPONENT"));
	ArrowSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKELETALMESHCOMPONENT"));
	RootComponent = ArrowSkeletalMeshComponent;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Script/Engine.SkeletalMesh'/Game/MedievalArmour/weapon/Props/Meshes/Arrow/Arrow.Arrow'"));
	if (SK_WEAPON.Succeeded())
	{
		ArrowSkeletalMeshComponent->SetSkeletalMesh(SK_WEAPON.Object);
	}
	ProjectileComp->MaxSpeed = 0.f;
	ProjectileComp->InitialSpeed = 0.f;
	
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();
	TargetLoc = GetActorLocation()+GetActorUpVector()*1000.f;
	ArrowSkeletalMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SphereComponent->AttachToComponent(ArrowSkeletalMeshComponent,FAttachmentTransformRules::KeepRelativeTransform,"HitSocket");
	SphereComponent->SetCollisionProfileName(TEXT("NoCollision"));
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlap);
	SphereComponent->SetSphereRadius(5.f);
	//SphereComponent->bHiddenInGame = false;

}

void AArrow::Attack()
{
	//ABow* CurrentBow = Cast<ABow>(GetAttachParentActor());
	//if(CurrentBow == nullptr) return;
	//Character = Cast<AAssassinCharacter>(CurrentBow->GetAttachParentActor());
	Character = Cast<AAssassinCharacter>(GetAttachParentActor());
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//SetActorRotation((TargetLoc-GetActorLocation()).Rotation());
	FVector StartLoc = GetActorLocation();
	//DrawDebugSphere(GetWorld(), TargetLoc, 17, 26, FColor( 0,0, 181), true, -1, 0, 2);
	float arcValue = 0.9f;                       // ArcParam (0.0-1.0)
	FVector outVelocity = FVector::ZeroVector;   
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, StartLoc, TargetLoc, GetWorld()->GetGravityZ(), arcValue))
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow Shoooooooooooot!!!!!!!!!!!!!!!!!: %s"),*outVelocity.ToString());
		//SetActorRotation(outVelocity.ToOrientationQuat());
		ArrowSkeletalMeshComponent->SetSimulatePhysics(true);
		//ArrowSkeletalMeshComponent->SetAllPhysicsLinearVelocity(outVelocity*1.f);
		//ProjectileComp->InitialSpeed = outVelocity.Size();
		//ProjectileComp->MaxSpeed = outVelocity.Size();
		ArrowSkeletalMeshComponent->AddImpulse(outVelocity);
		
		SphereComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	}
	
}

void AArrow::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == this||Character == nullptr|| Character == OtherActor) return;
	UE_LOG(LogTemp, Warning, TEXT("overlap!!! %s Hit Actor: %s"),*OverlappedComponent->GetName(), *OtherActor->GetName());

	ArrowSkeletalMeshComponent->bApplyImpulseOnDamage = false;
	ArrowSkeletalMeshComponent->SetSimulatePhysics(false);
	ArrowSkeletalMeshComponent->SetGenerateOverlapEvents(false);
	ArrowSkeletalMeshComponent->SetNotifyRigidBodyCollision(false);
	ArrowSkeletalMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	if(Cast<AAssassinCharacter>(OtherActor))
	{
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(50.f, DamageEvent, nullptr, this);
	}

	
	//DrawDebugSphere(GetWorld(), SweepResult.Location, 15, 26, FColor( 0,181, 0), true, -1, 0, 2);
	Destroy();
}


