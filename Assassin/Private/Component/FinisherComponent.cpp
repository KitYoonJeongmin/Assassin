// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FinisherComponent.h"
#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Character/ACAnimInstance.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values for this component's properties
UFinisherComponent::UFinisherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//Finish
	IsFinishing = false;
}


// Called when the game starts
void UFinisherComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFinisherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFinisherComponent::PlayFinish(AAssassinCharacter* PlayerCharacter, AAssassinCharacter* TargetCharacter)
{
	IsFinishing = true;
	UAnimMontage* FinisherMon = PlayerCharacter->ACAnim->PlayFinisherMontage();
	if (FinisherMon == nullptr) return;
	MoveToActor(PlayerCharacter,TargetCharacter, 110.f);
	FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(TargetCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	TargetCharacter->ACAnim->PlayFinishedMontage(FinisherMon);
	TargetCharacter->SetActorRotation(TargetRot);
	
}

void UFinisherComponent::MoveToActor(AAssassinCharacter* PlayerCharacter, AActor* TargetActor, float Distance)
{
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(PlayerCharacter->GetActorLocation(), TargetActor->GetActorLocation());
	LookAtRot.Pitch = PlayerCharacter->GetActorRotation().Pitch;

	FVector FrontFromActor = TargetActor->GetActorLocation();
	FVector CharacterToActorDirection = TargetActor->GetActorLocation() - PlayerCharacter->GetActorLocation();
	CharacterToActorDirection.Z = 0;
	CharacterToActorDirection.Normalize();
	FrontFromActor -= CharacterToActorDirection * Distance;


	FLatentActionInfo Info;
	Info.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(PlayerCharacter->GetCapsuleComponent(), FrontFromActor, LookAtRot, false, false, 0.2f, false, EMoveComponentAction::Type::Move, Info);
}
void UFinisherComponent::SwitchCamera(APlayerCharacter* PlayerCharacter, bool UseSequence)
{
	APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	if (PlayerController)
	{
		// 현재 사용 중인 카메라 비활성화
		if (UseSequence)
		{
			PlayerController->SetViewTargetWithBlend(PlayerCharacter->SequenceCameraChildActor->GetChildActor(), 0.5f);
		}
		else
		{
			PlayerController->SetViewTargetWithBlend(PlayerCharacter->FollowCameraChildActor->GetChildActor(), 0.3f);
		}
	}
}
