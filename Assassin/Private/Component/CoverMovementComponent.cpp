// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CoverMovementComponent.h"

#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UCoverMovementComponent::UCoverMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsCover = false;
}

void UCoverMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
}


// Called when the game starts
void UCoverMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AAssassinCharacter>(GetOwner());
	if(Character==nullptr)
	{
		//UE_LOG(LogTemp,Warning,TEXT("-----Cover Character is null-------"));
	}
	// ...
	
}


// Called every frame
void UCoverMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!IsCover) return;
	//커버 상태일 때 등 뒤에 벽이 없다면 다시 false로
	FVector StartLoc = Character->GetActorLocation() + Character->GetActorForwardVector()*-50.f;
	FVector EndLoc = StartLoc;
	float Radius = 30.f;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	FHitResult HitResult;
	if(!UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLoc, EndLoc, Radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
	{
		DisableCover();
	}

	//속도가 올라간다면 false로
	if(Character->GetCharacterMovement()->MaxWalkSpeed > 300.f)
	{
		DisableCover();
	}
}

void UCoverMovementComponent::ToggleCoverMovement()
{
	if(Character == nullptr)
	{
		//UE_LOG(LogTemp,Warning,TEXT("-----Cover-------"));
		return;
	}
	
	FVector StartLoc = Character->GetActorLocation()+Character->GetActorForwardVector()*50.f;
	FVector EndLoc = StartLoc + Character->GetActorForwardVector()*70.f;
	float Radius = 50.f;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	FHitResult HitResult;
	if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLoc, EndLoc, Radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
	{
		EnableCover(HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
	else
	{
		DisableCover();
	}
}

void UCoverMovementComponent::EnableCover(FVector WallLoc, FVector WallRot)
{
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->SetWalkSpeed(100.f);
	Character->GetCharacterMovement()->MaxWalkSpeed = 100.f;
	//IsCover = true;
	Character->ACAnim->IsCover = true;
	Character->ACAnim->UpdateMovementState(EMovementState::E_Covering);
	Character->UpdateMovementState(EMovementState::E_Covering);
	WallRot.Z = Character->GetActorRotation().Pitch;

	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(Character->GetCapsuleComponent(), WallLoc+WallRot*30.f, WallRot.Rotation(), false, false, 0.2f, false, EMoveComponentAction::Type::Move, Info);

	FTimerHandle GravityTimerHandle;
	float GravityTime = 0.2f;

	GetWorld()->GetTimerManager().SetTimer(GravityTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		IsCover = true;
		GetWorld()->GetTimerManager().ClearTimer(GravityTimerHandle);
	}), GravityTime, false);

}

void UCoverMovementComponent::DisableCover()
{
	Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	Character->SetWalkSpeed(300.f);
	
	
	IsCover = false;
	Character->ACAnim->IsCover = false;
	Character->ACAnim->UpdateMovementState(EMovementState::E_Walking);
	Character->UpdateMovementState(EMovementState::E_Walking);
}

void UCoverMovementComponent::MoveInput(FVector2d Value)
{
	/*FVector InputRightVector;
	InputRightVector = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::Y)*Value.Get<FVector2D>().Y;
	InputRightVector.Normalize();
	FVector::DotProduct(InputRightVector, GetActorRightVector());
	CoverMovementComponent->MoveInput(Value.Get<FVector2D>());*/

	FVector StartLoc = Character->GetActorLocation()+ Character->GetActorRightVector()*-Value.X*100.f;
	FVector EndLoc = StartLoc+ Character->GetActorForwardVector()*-100.f;
	float Radius = 50.f;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	FHitResult HitResult;
	if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLoc, EndLoc, Radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
	{
		Character->ACAnim->CoverMoveInput = -Value.X;
		Character->AddMovementInput(Character->GetActorRightVector(),-Value.X);
	}
	else
	{
		Character->ACAnim->CoverMoveInput = 0.f;
	}
}


