// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"
#include "ACAnimInstance.h"
#include "AssassinCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Arrows
	FrontWallTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontWallArrow"));
	LeftMoveTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("RightArrow"));
	RightMoveTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftArrow"));
	ClimbUpTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	WallHeightDownTraceLimit = 0;
	WallHeightUpTraceLimit = 90.f;

	LedgeHightLocationXY = 38.f;
	LedgeHightLocationZ = 100.f;



}


// Called when the game starts
void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();

	//Initializing Character Component
	Character = Cast<AAssassinCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	CharMoveComp = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	CharCapsuleComp = Character->GetCapsuleComponent();
	CharMeshComp = Character->GetMesh();
	CharAnimInstance = Cast<UACAnimInstance>(CharMeshComp->GetAnimInstance());

	//Arrow
	FrontWallTracer->AttachToComponent(Character->GetCapsuleComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	FrontWallTracer->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	LeftMoveTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	LeftMoveTracer->SetRelativeLocation(FVector(0.f, -20.f, 100.f));
	RightMoveTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	RightMoveTracer->SetRelativeLocation(FVector(0.f, 20.f, 100.f));
	ClimbUpTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	ClimbUpTracer->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	ClimbUpTracer->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//if (ClimbingState.CanClimbOnLedge)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Can"));
	//}
	//else
	//	UE_LOG(LogTemp, Log, TEXT("Cannot"));
}

void UClimbingComponent::GetWallLocation(FVector StartPoint)
{
	TArray<AActor*> ActorsToIgnore;

	if(Character == nullptr) { UE_LOG(LogTemp, Log, TEXT("Character Null")); }
	else
	{
		ActorsToIgnore.Add(Character);
	}
	StartPoint.X = FrontWallTracer->GetComponentLocation().X;
	StartPoint.Y = FrontWallTracer->GetComponentLocation().Y;
	bool IsForward = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartPoint,
		StartPoint +FrontWallTracer->GetForwardVector()*100.f,	//100은 얼마나 멀리있는 벽을 잡을 것인가
		10,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		ActorsToIgnore, 
		EDrawDebugTrace::ForDuration,
		ForwardHitResult,
		false
		, FLinearColor::Red
		, FLinearColor::Green
		, 5.0f
	);
	if (IsForward)	//앞에 벽이 있다면
	{
		WallImpactNormal = ForwardHitResult.ImpactNormal;
		WallImpactPoint = ForwardHitResult.ImpactPoint;
		ClimbRotation = UKismetMathLibrary::MakeRotFromX(-1 * WallImpactNormal);

		LedgeLocation = WallImpactPoint - UKismetMathLibrary::GetForwardVector(ClimbRotation) * LedgeHightLocationXY;
		LedgeLocation.Z = (HightHitResult.ImpactPoint.Z - LedgeHightLocationZ);
		//GetWallHeight(WallImpactPoint,Character->GetActorForwardVector());

		ClimbingState.CanClimbOnLedge = true;
		ClimbingState.ClimbUpLedge = false;
		ClimbingState.DropDown = false;
		ClimbingState.HighMantle = false;
	}
	else
	{
		ClimbingState.CanClimbOnLedge = false;
		ClimbingState.ClimbUpLedge = false;
		ClimbingState.DropDown = false;
		ClimbingState.HighMantle = false;
	}
}

void UClimbingComponent::GetWallHeight()
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	bool IsHeight = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		FrontWallTracer->GetComponentLocation() + FVector(40.f,0.f,WallHeightUpTraceLimit),
		FrontWallTracer->GetComponentLocation() + FVector(40.f, 0.f, WallHeightDownTraceLimit),
		10,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HightHitResult,
		true
		, FLinearColor::Red
		, FLinearColor::Green
		, 5.0f
	);
	
	if (IsHeight)	//위에 벽이 있다면
	{
		FVector StartPoint = HightHitResult.ImpactPoint;
		bool IsEnd = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			StartPoint + FVector(0.f,0.f,20.f),
			StartPoint + FVector(0.f, 0.f, 20.f) + 10.f*Character->GetActorForwardVector(),
			5,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true
			, FLinearColor::Red
			, FLinearColor::Green
			, 5.0f
		);
		if (!IsEnd)	//충분히 잡을 수 있음
		{
			/*ClimbingState.CanClimbOnLedge = true;
			ClimbingState.ClimbUpLedge = false;
			ClimbingState.DropDown = false;
			ClimbingState.HighMantle = false;*/
			GetWallLocation(HightHitResult.ImpactPoint);
		}
		else //잡을 수 없음
		{
			ClimbingState.CanClimbOnLedge = false;
			ClimbingState.ClimbUpLedge = false;
			ClimbingState.DropDown = false;
			ClimbingState.HighMantle = false;
		}

		
	}
}

void UClimbingComponent::GrabLedge()
{
	float OverTime;
	OverTime = UKismetMathLibrary::Vector_Distance(LedgeLocation, Character->GetActorLocation());
	SetOnWall(OverTime);
}

void UClimbingComponent::SetOnWall(float OverTime)
{
	CharCapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CharMoveComp->SetMovementMode(MOVE_Flying);
	CharMoveComp->bOrientRotationToMovement = false;
	switch(Character->CurrnetMovementState)
	{
	case EMovementState::E_Walking:
		Character->UpdateMovementState(EMovementState::E_Hanging);
		break;
	}
	if (true)
	{
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(
			CharCapsuleComp,
			LedgeLocation,
			ClimbRotation,
			false,
			false,
			(OverTime/2000.f),
			false,
			EMoveComponentAction::Type::Move,
			Info);
	}
}

bool UClimbingComponent::LedgeMoveLeft()
{
	FVector StartPos = LeftMoveTracer->GetComponentLocation();
	FVector EndPos = LeftMoveTracer->GetComponentLocation() + RightMoveTracer->GetForwardVector() * 50;

	FVector ImpactPoint1;
	FVector ImpactPoint2;
	FRotator Rot;

	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	bool IsRight = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartPos,
		EndPos,
		10,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
		, FLinearColor::Red
		, FLinearColor::Green
		, 5.0f
	);
	if (IsRight)
	{
		ImpactPoint1 = HitResult.ImpactPoint;
		Rot = UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal);
		StartPos = HitResult.ImpactPoint + FVector(0.f, 0.f, 20.f);
		EndPos = StartPos + FVector(0.f, 0.f, -20.f);
		bool IsRight2 = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			StartPos,
			EndPos,
			10,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true
			, FLinearColor::Red
			, FLinearColor::Green
			, 5.0f
		);
		if (IsRight2)
		{
			ImpactPoint2 = HitResult.ImpactPoint;
			StartPos = HitResult.ImpactPoint + Character->GetActorRightVector() * (-28.f);
			EndPos = StartPos;
			bool IsRight3 = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				StartPos,
				EndPos,
				10,
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
				false,
				ActorsToIgnore,
				EDrawDebugTrace::ForOneFrame,
				HitResult,
				true
				, FLinearColor::Red
				, FLinearColor::Green
				, 5.0f
			);
			if (IsRight3)
			{
				MoveOnLedge(ImpactPoint1, ImpactPoint2, Rot);
				return IsRight3;
			}
		}
	}
	return false;
}

bool UClimbingComponent::LedgeMoveRight()
{
	FVector StartPos = RightMoveTracer->GetComponentLocation();
	FVector EndPos = RightMoveTracer->GetComponentLocation()+ RightMoveTracer->GetForwardVector()*50;

	FVector ImpactPoint1;
	FVector ImpactPoint2;
	FRotator Rot;

	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	bool IsRight = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartPos,
		EndPos,
		10,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
		, FLinearColor::Red
		, FLinearColor::Green
		, 5.0f
	);
	if (IsRight)
	{
		ImpactPoint1 = HitResult.ImpactPoint;
		Rot = UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal);
		StartPos = HitResult.ImpactPoint + FVector(0.f, 0.f, 20.f);
		EndPos = StartPos + FVector(0.f, 0.f, -20.f);
		bool IsRight2 = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			StartPos,
			EndPos,
			10,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true
			, FLinearColor::Red
			, FLinearColor::Green
			, 5.0f
		);
		if (IsRight2)
		{
			ImpactPoint2 = HitResult.ImpactPoint;
			StartPos = HitResult.ImpactPoint + Character->GetActorRightVector()*28.f;
			EndPos = StartPos;
			bool IsRight3 = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				StartPos,
				EndPos,
				10,
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
				false,
				ActorsToIgnore,
				EDrawDebugTrace::ForOneFrame,
				HitResult,
				true
				, FLinearColor::Red
				, FLinearColor::Green
				, 5.0f
			);
			if (IsRight3)
			{
				MoveOnLedge(ImpactPoint1, ImpactPoint2, Rot);
				return IsRight3;
			}
		}
	}
	return false;
}

void UClimbingComponent::MoveOnLedge(FVector T1ImpactPoint, FVector T2ImpactPoint, FRotator Rot)
{
	//이동할 위치
	FVector MoveLoc = UKismetMathLibrary::GetForwardVector(Rot) * LedgeHightLocationXY + T1ImpactPoint;
	MoveLoc.Z = (T2ImpactPoint.Z - LedgeHightLocationZ);
	FVector CharLoc = Character->GetActorLocation();
	FVector InterLoc = UKismetMathLibrary::VInterpTo(CharLoc, MoveLoc, GetWorld()->DeltaTimeSeconds, 2);

	//이동할 rot
	FRotator MoveRot = Rot;
	MoveRot.Yaw -= 180;
	FRotator CharRot = Character->GetActorRotation();
	FRotator InterRot = UKismetMathLibrary::RInterpTo(CharRot, MoveRot, GetWorld()->DeltaTimeSeconds, 5);

	Character->SetActorLocationAndRotation(InterLoc, InterRot);
}

void UClimbingComponent::ClimbUp()
{
	FVector StartPoint = ClimbUpTracer->GetComponentLocation() + 50.f*Character->GetActorForwardVector();
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	//올라갈 수 있는가?
	bool CanNotClimbUp = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartPoint ,
		StartPoint + 150.f * ClimbUpTracer->GetForwardVector(),
		10,
		//UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_EngineTraceChannel1),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
		, FLinearColor::Blue
		, FLinearColor::Gray
		, 5.0f
	);
	if (!CanNotClimbUp)	//올라갈 수 있으면
	{
		ClimbingState.CanClimbOnLedge = false;
		ClimbingState.ClimbUpLedge = true;
		ClimbingState.DropDown = false;
		ClimbingState.HighMantle = false;
	}
}

void UClimbingComponent::Fall()
{
	CharMoveComp->SetMovementMode(MOVE_Walking);
	CharMoveComp->bOrientRotationToMovement = true;
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Character->UpdateMovementState(EMovementState::E_Walking);
	Character->ACAnim->UpdateMovementState(EMovementState::E_Walking);

	ClimbingState.CanClimbOnLedge = false;
	ClimbingState.ClimbUpLedge = false;
	ClimbingState.DropDown = false;
	ClimbingState.HighMantle = false;
}


