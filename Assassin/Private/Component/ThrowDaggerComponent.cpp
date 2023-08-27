// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ThrowDaggerComponent.h"

#include "ThrowdaggerAnimWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "Character/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/ThrowDagger.h"

UThrowDaggerComponent::UThrowDaggerComponent()
{
	ThrowDaggerClass = AThrowDagger::StaticClass();
	static ConstructorHelpers::FObjectFinder<UBlueprint> BP_Dagger(TEXT("/Script/Engine.Blueprint'/Game/AMainDirectory/Weapon/MyThrowDagger.MyThrowDagger'"));
	if (BP_Dagger.Object)
	{
		ThrowDaggerClass = (UClass*)BP_Dagger.Object->GeneratedClass;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_ANIM(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/AMainDirectory/UI/RangeWeapon/UI_Aim.UI_Aim_C'"));
	if (UI_ANIM.Succeeded())
	{
		AimUIClass = UI_ANIM.Class;
	}
}

void UThrowDaggerComponent::BeginPlay()
{
	Super::BeginPlay();
	AimUIObject = Cast<UThrowdaggerAnimWidget>(CreateWidget(GetWorld(), AimUIClass));
	AimUIObject->AddToViewport();
	AimUIObject->SetVisibility(ESlateVisibility::Hidden);

	OwnerCharacter->ACAnim->OnSpawnDagger.AddDynamic(this,&UThrowDaggerComponent::GenerateDagger);
}

void UThrowDaggerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AnimTrace();
}

void UThrowDaggerComponent::DrawGuide()
{
	SetIsZooming(true);
	AimUIObject->SetVisibility(ESlateVisibility::Visible);
}

void UThrowDaggerComponent::DeleteGuide()
{
	IsZooming=false;
	AimUIObject->SetVisibility(ESlateVisibility::Hidden);
}

void UThrowDaggerComponent::ShootProjectile()
{
	if(!IsZooming) return;
	FRotator newRot = OwnerCharacter->GetActorRotation();
	newRot.Yaw =  OwnerCharacter->GetControlRotation().Yaw;
	newRot.Yaw += 10.f;
	OwnerCharacter->SetActorRotation(newRot);
	OwnerCharacter->ACAnim->PlayThrowDaggerMontage();
}

void UThrowDaggerComponent::AnimTrace()
{
	if(!IsZooming) return;
	FVector StartLoc = OwnerCharacter->FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc + OwnerCharacter->GetControlRotation().Vector()*50000.f;
	
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter);
	bool IsHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		UEngineTypes::ConvertToTraceType(ECC_PhysicsBody),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
		, FLinearColor::Gray
		, FLinearColor::Blue
		, 3.0f
	);

	if(IsHit && Cast<AEnemy>(HitResult.GetActor()))
	{
		TargetLoc = HitResult.Location;
		AimUIObject->SetAnimColor(false);
	}
	else
	{
		TargetLoc = EndLoc;
		AimUIObject->SetAnimColor(true);
	}
}

void UThrowDaggerComponent::GenerateDagger()
{
	AThrowDagger* ThrowDagger = GetWorld()->SpawnActor<AThrowDagger>(ThrowDaggerClass, OwnerCharacter->FollowCamera->GetComponentLocation()+OwnerCharacter->FollowCamera->GetForwardVector()*700.f, OwnerCharacter->GetControlRotation());
	ThrowDagger->OwnerCharacter = OwnerCharacter;
	ThrowDagger->Shoot(TargetLoc);
}
