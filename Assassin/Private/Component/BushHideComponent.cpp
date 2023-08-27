// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BushHideComponent.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Character/ACAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "Component/ClimbingComponent.h"
#include "Component/EagleVisionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/Dagger.h"
// Sets default values for this component's properties
UBushHideComponent::UBushHideComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	BushDetectionBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BushDetectionBoxComp"));
	BushDetectionBoxComp->OnComponentBeginOverlap.AddDynamic(this, &UBushHideComponent::OnBoxComponentBeginOverlap);
	BushDetectionBoxComp->OnComponentEndOverlap.AddDynamic(this, &UBushHideComponent::OnBoxComponentEndOverlap);


	SightBlockComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("SightBlockActorComp"));
	
	BushNum = 0;
	
}


// Called when the game starts
void UBushHideComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BushDetectionBoxComp->SetCollisionProfileName("BushCollision");
	BushDetectionBoxComp->AttachToComponent(GetOwner()->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	BushDetectionBoxComp->SetRelativeLocation(FVector(0.f,0.f,-20.f));
	

	SightBlockComp->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),FAttachmentTransformRules::KeepRelativeTransform);
	SightBlockComp->SetRelativeLocation(FVector(0.f,0.f,0.f));
	if(SightBlockComp->GetChildActor())
	{
		SightBlockComp->GetChildActor()->SetActorEnableCollision(false);
	}
	

	Character = Cast<APlayerCharacter>(GetOwner());
}


// Called every frame
void UBushHideComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Tick")));
	/*
	if(Character->GetVelocity().Size()>400.f && Character->CurrnetMovementState == EMovementState::E_Stealthing)
	{
		DisableStealthMode();
	}*/
}

void UBushHideComponent::OnBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	++BushNum;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Overlap start<%s>: %d"),*(OtherActor->GetName()), BushNum));
	UE_LOG(LogTemp, Warning, TEXT("Overlap start<%s>: %d"),*(OtherActor->GetName()), BushNum);
	if(Character->GetCurrentWeapon() != Character->Weapon.DaggleWeapon) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = 100.f;
	

	if(!(SightBlockComp->GetChildActor()->GetActorEnableCollision()))
	{
		EnableStealthMode();
	}
}

void UBushHideComponent::OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	--BushNum;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Overlap End<%s>: %d"),*(OtherActor->GetName()), BushNum));
	UE_LOG(LogTemp, Warning, TEXT("Overlap End<%s>: %d"),*(OtherActor->GetName()), BushNum);

	if(Character->GetVelocity().Size()>400.f) return;
	

	if(BushNum<=0)
	{
		DisableStealthMode();
	}
}

void UBushHideComponent::EnableStealthMode()
{
	SightBlockComp->GetChildActor()->SetActorEnableCollision(true);
	Character->ACAnim->UpdateMovementState(EMovementState::E_Stealthing);
	Character->UpdateMovementState(EMovementState::E_Stealthing);
}

void UBushHideComponent::DisableStealthMode()
{
	BushNum = 0;
	SightBlockComp->GetChildActor()->SetActorEnableCollision(false);
	Character->ACAnim->UpdateMovementState(EMovementState::E_Walking);
	Character->UpdateMovementState(EMovementState::E_Walking);
}

