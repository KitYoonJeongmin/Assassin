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
#include "Math/Vector.h"

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

	//Grid Arrows
	UpGridTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("UpLedgeArrow"));
	RightGridTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("RightLedgeArrow"));
	LeftGridTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftLedgeArrow"));
	DownGridTracer = CreateDefaultSubobject<UArrowComponent>(TEXT("DownLedgeArrow"));

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
	LeftMoveTracer->SetRelativeLocation(FVector(0.f, -30.f, 100.f));
	RightMoveTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	RightMoveTracer->SetRelativeLocation(FVector(0.f, 30.f, 100.f));
	ClimbUpTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	ClimbUpTracer->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	ClimbUpTracer->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	UpGridTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	LeftGridTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	RightGridTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DownGridTracer->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	UpGridTracer->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	LeftGridTracer->SetRelativeLocation(FVector(0.f, -150.f, 50.f));
	RightGridTracer->SetRelativeLocation(FVector(0.f, 150.f, 50.f));
	DownGridTracer->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (ClimbingState.CanClimbOnLedge )
	{
		HandIK();
	}
	
}

void UClimbingComponent::GetWallLocation(FVector StartPoint)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
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
		EDrawDebugTrace::None,
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

		ClimbingState.CanClimbOnLedge = true;
		ClimbingState.CanMoveOnLedge = false;
		ClimbingState.ClimbUpLedge = false;
		ClimbingState.DropDown = false;
		ClimbingState.HighMantle = false;
		ClimbingState.CanMoveToSide = false;
	}
	else
	{
		ClimbingState.CanClimbOnLedge = false;
		ClimbingState.CanMoveOnLedge = false;
		ClimbingState.ClimbUpLedge = false;
		ClimbingState.DropDown = false;
		ClimbingState.HighMantle = false;
		ClimbingState.CanMoveToSide = false;
	}
}

void UClimbingComponent::GetWallHeight()
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	bool IsHeight = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		FrontWallTracer->GetComponentLocation() + FrontWallTracer->GetForwardVector()*40.f+ FVector(0.f,0.f,WallHeightUpTraceLimit),
		FrontWallTracer->GetComponentLocation() + FrontWallTracer->GetForwardVector() * 40.f + FVector(0.f, 0.f, WallHeightDownTraceLimit),
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
			EDrawDebugTrace::None,
			HitResult,
			true
			, FLinearColor::Red
			, FLinearColor::Green
			, 5.0f
		);
		if (!IsEnd)	//충분히 잡을 수 있음
		{
			GetWallLocation(HightHitResult.ImpactPoint);
		}
		else //잡을 수 없음
		{
			ClimbingState.CanClimbOnLedge = false;
			ClimbingState.CanMoveOnLedge = false;
			ClimbingState.ClimbUpLedge = false;
			ClimbingState.DropDown = false;
			ClimbingState.HighMantle = false;
			ClimbingState.CanMoveToSide = false;
		}

		
	}
}

void UClimbingComponent::MoveToLedge()
{
	float OverTime;
	OverTime = UKismetMathLibrary::Vector_Distance(LedgeLocation, Character->GetActorLocation());
	//ClimbingState.CanMoveOnLedge = false;
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
			(OverTime/800.f),
			false,
			EMoveComponentAction::Type::Move,
			Info);
	}
}

////////MoveTo Side

bool UClimbingComponent::LedgeMoveLeft()
{
	if (Character->MovementVector.Y != 0) return false;
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
			EndPos = StartPos+Character->GetActorForwardVector()*20.f;
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
				ClimbingState.CanClimbOnLedge = true;
				ClimbingState.ClimbUpLedge = false;
				ClimbingState.DropDown = false;
				ClimbingState.HighMantle = false;
				ClimbingState.CanMoveToSide = true;
				MoveOnLedge(ImpactPoint1, ImpactPoint2, Rot);
				return ClimbingState.CanMoveToSide;
			}
		}
	}
	ClimbingState.CanClimbOnLedge = true;
	ClimbingState.ClimbUpLedge = false;
	ClimbingState.DropDown = false;
	ClimbingState.HighMantle = false;
	ClimbingState.CanMoveToSide = false;
	return ClimbingState.CanMoveToSide;
}

bool UClimbingComponent::LedgeMoveRight()
{
	if (Character->MovementVector.Y != 0) return false;
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
		UE_LOG(LogTemp, Warning, TEXT("Rot: % f, % f, % f"), HitResult.ImpactNormal.X, HitResult.ImpactNormal.Y, HitResult.ImpactNormal.Z);
		Rot = UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal);
		//UE_LOG(LogTemp, Warning, TEXT("Rot: % f, % f, % f"), Rot.Roll, Rot.Pitch, Rot.Yaw);
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
			EndPos = StartPos + Character->GetActorForwardVector() * 20.f;
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
				ClimbingState.CanClimbOnLedge = true;
				ClimbingState.ClimbUpLedge = false;
				ClimbingState.DropDown = false;
				ClimbingState.HighMantle = false;
				ClimbingState.CanMoveToSide = true;

				MoveOnLedge(ImpactPoint1, ImpactPoint2, Rot);
				return ClimbingState.CanMoveToSide;
			}
		}
	}
	ClimbingState.CanClimbOnLedge = true;
	ClimbingState.ClimbUpLedge = false;
	ClimbingState.DropDown = false;
	ClimbingState.HighMantle = false;
	ClimbingState.CanMoveToSide = false;
	return ClimbingState.CanMoveToSide;
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
	FRotator InterRot = UKismetMathLibrary::RInterpTo(CharRot, MoveRot, GetWorld()->DeltaTimeSeconds, 2);

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
		EDrawDebugTrace::None,
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
		ClimbingState.CanMoveToSide = false;
	}
	else
	{
		ClimbingState.CanClimbOnLedge = true;
		ClimbingState.ClimbUpLedge = false;
		ClimbingState.DropDown = false;
		ClimbingState.HighMantle = false;
		ClimbingState.CanMoveToSide = false;
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
	ClimbingState.CanMoveToSide = false;
}

void UClimbingComponent::DropToHang()
{
	FVector FeetLoc = (Character->MeshArr[1]->GetSocketLocation(FName("ball_l")) + Character->MeshArr[1]->GetSocketLocation(FName("ball_r"))) / 2.f;
	FVector StartLoc = FeetLoc + Character->GetActorUpVector() * 20;
	FVector EndLoc = FeetLoc - Character->GetActorUpVector() * 20;
	FHitResult UpHitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	bool CanDrop = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		20,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		UpHitResult,
		true
		, FLinearColor::Red
		, FLinearColor::Green
		, 5.0f
	);

	if (CanDrop)
	{
		FHitResult SideHitResult;
		StartLoc = UpHitResult.ImpactPoint - 30.f * UpHitResult.GetActor()->GetActorForwardVector();
		EndLoc = UpHitResult.ImpactPoint;
		bool CanHang = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			StartLoc,
			EndLoc,
			10,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			SideHitResult,
			true
			, FLinearColor::Blue
			, FLinearColor::Gray
			, 5.0f
		);
		if (CanHang)
		{
			ClimbRotation = UKismetMathLibrary::MakeRotFromX(-1 * SideHitResult.ImpactNormal);
			LedgeLocation = SideHitResult.ImpactPoint - UKismetMathLibrary::GetForwardVector(ClimbRotation) * LedgeHightLocationXY;
			LedgeLocation.Z = (UpHitResult.ImpactPoint.Z - LedgeHightLocationZ);

			Character->CurrnetMovementState = EMovementState::E_Hanging;
			for (auto mesh : Character->MeshArr)
			{
				Cast<UACAnimInstance>(mesh->GetAnimInstance())->CurrnetMovementState = EMovementState::E_Hanging;
			}
			ClimbingState.CanClimbOnLedge = true;
			ClimbingState.CanMoveOnLedge = false;
			ClimbingState.ClimbUpLedge = false;
			ClimbingState.DropDown = false;
			ClimbingState.HighMantle = false;
			ClimbingState.CanMoveToSide = false;
			MoveToLedge();
		}
	}
}



/// <summary>
/// 매달린 상태에서 다른 난간으로 이동하기 위해 가까운 난간을 찾는 함수
/// </summary>
/// <param name="Right"></param>
/// <param name="Up"></param>

void UClimbingComponent::FindLedge(float Right, float Up)
{
	
	FVector CenterLoc = Character->GetActorLocation();
	if (Right != 0)
	{
		if (Right > 0)	//right
		{
			CenterLoc += Character->GetActorRightVector()*RightGridTracer->GetRelativeLocation().Y;
		}
		else  //left
		{
			CenterLoc += Character->GetActorRightVector() * LeftGridTracer->GetRelativeLocation().Y;
		}
	}
	if (Up != 0)
	{
		if (Up > 0)	//up
		{
			CenterLoc += Character->GetActorUpVector() * UpGridTracer->GetRelativeLocation().Z;
		}
		else  //down
		{
			CenterLoc += Character->GetActorUpVector() * DownGridTracer->GetRelativeLocation().Z;
		}
	}
	else
	{
		CenterLoc.Z += 50;
	}
	
	double LedgeDistance = 999999999.f;
	FVector DebugLoc;
	FVector HandLoc = (Character->MeshArr[0]->GetSocketLocation(FName("hand_lSocket")) + Character->MeshArr[0]->GetSocketLocation(FName("hand_rSocket"))) / 2.f;
	for (int8 i = -7; i < 7; i++)
	{
		for (int8 j = -7; j < 7; j++)
		{
			FVector CurrentLedgeLoc;	//현재 Ledge의 위치정보
			FRotator CurrentLedgeRot;

			FHitResult SideHitResult;	//LineTrace정보
			FHitResult UpHitResult;		//LineTrace정보

			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(Character);
			FVector StartLoc = CenterLoc;
			StartLoc += Character->GetActorRightVector() * 10 * i;
			StartLoc += Character->GetActorUpVector() * 10 * j;
			bool IsLedge = UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				StartLoc - 50.f * Character->GetActorForwardVector(),
				StartLoc + 50.f * Character->GetActorForwardVector(),
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
				false,
				ActorsToIgnore,
				EDrawDebugTrace::ForDuration,
				SideHitResult,
				true
				, FLinearColor::Gray
				, FLinearColor::Blue
				, 5.0f
			);
			if (!IsLedge) { continue; }
			
			CurrentLedgeRot = UKismetMathLibrary::MakeRotFromX(-1 * SideHitResult.ImpactNormal);
			CurrentLedgeLoc = WallImpactPoint - UKismetMathLibrary::GetForwardVector(CurrentLedgeRot) * LedgeHightLocationXY;
			WallImpactNormal = SideHitResult.ImpactNormal;
			WallImpactPoint = SideHitResult.ImpactPoint;

			//Y로 추적
			bool IsLedgeHight = UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				WallImpactPoint + 40.f * Character->GetActorUpVector(),
				WallImpactPoint,
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
				false,
				ActorsToIgnore,
				EDrawDebugTrace::ForDuration,
				UpHitResult,
				true
				, FLinearColor::Green
				, FLinearColor::Red
				, 3.0f
			);
			if (!IsLedgeHight) { continue; }

			CurrentLedgeLoc.Z = (UpHitResult.ImpactPoint.Z - LedgeHightLocationZ);	//최종 LedgeLoc 구함

			double dist = FVector::Dist(HandLoc, UpHitResult.ImpactPoint);	//해당 위치와 손의 거리구함
			//UE_LOG(LogTemp, Warning, TEXT("%dLedge dist: %f"),i+j, dist);	
			if (dist > LedgeDistance) { continue; }	//거리가 최단거리라면 
			LedgeDistance = dist;	//업데이트

			ClimbRotation = CurrentLedgeRot;
			LedgeLocation = CurrentLedgeLoc;
			ClimbingState.CanClimbOnLedge = true;
			ClimbingState.CanMoveOnLedge = true;
			ClimbingState.ClimbUpLedge = false;
			ClimbingState.DropDown = false;
			ClimbingState.HighMantle = false;
			ClimbingState.CanMoveToSide = false;
			
			
			//DrawDebugSphere(GetWorld(), UpHitResult.ImpactPoint, 10, 10, FColor(0, 0, 181), false, 2.f);
			//DebugLoc = UpHitResult.ImpactPoint;
			//UE_LOG(LogTemp, Warning, TEXT("Ledge Point: %f, %f, %f"), CurrentLedgeLoc.X, CurrentLedgeLoc.Y, CurrentLedgeLoc.Z);
		}
	}
	/* ------------Debug!!!!-------------------
	UE_LOG(LogTemp, Warning, TEXT("CurrentDis:%f--------------------------"), LedgeDistance);
	if (ClimbingState.CanMoveOnLedge)
	{
		DrawDebugSphere(GetWorld(), DebugLoc, 10, 10, FColor(181, 0, 0), false, 2.f);
		DrawDebugSphere(GetWorld(), LedgeLocation, 20, 10, FColor(0, 181, 0), false, 2.f);
		DrawDebugSphere(GetWorld(), HandLoc, 10, 10, FColor(255, 255, 255), false, 2.f);
	}*/
	if (ClimbingState.CanMoveOnLedge)
	{
		Character->GetController()->DisableInput(Cast<APlayerController>(Character->GetController()));
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
			{
				Character->GetController()->EnableInput(Cast<APlayerController>(Character->GetController()));
				ClimbingState.CanMoveOnLedge = false;
			});

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.5, false);
	}
	return; 
}


/// <summary>
/// HandIK
/// </summary>
void UClimbingComponent::HandIK()
{
	FHitResult HitResult1;
	FHitResult HitResult2;
	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel6));

	FVector LeftStartVec = FrontWallTracer->GetComponentLocation() - Character->GetActorRightVector() * 7;
	FVector LeftEndVec = LeftStartVec + Character->GetActorForwardVector() * 80.f;
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), LeftStartVec, LeftEndVec, 5.f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
		false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult1, true))
	{
		FVector HitPoint(HitResult1.ImpactPoint);
		FHitResult HitResultLeft;
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), HitPoint + FVector(0.f, 0.f, 20.f), HitPoint, 5.f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
			false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResultLeft, true))
		{
			IKLeftHand = HitPoint +Character->GetActorForwardVector() * 5;
			IKLeftHand.Z = HitResultLeft.ImpactPoint.Z-5.f;
		}
	}

	FVector RightStartVec = FrontWallTracer->GetComponentLocation() + Character->GetActorRightVector()*7;
	FVector RightEndVec = RightStartVec + Character->GetActorForwardVector() * 80.f;
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), RightStartVec, RightEndVec, 5.f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), 		
		false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult2, true))
	{
		FVector HitPoint(HitResult2.ImpactPoint);
		FHitResult HitResultRight;
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), HitPoint + FVector(0.f, 0.f, 20.f), HitPoint, 5.f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
			false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResultRight, true))
		{
			IKRightHand = HitPoint+Character->GetActorForwardVector()*5;
			IKRightHand.Z = HitResultRight.ImpactPoint.Z-5.f;
		}
	}
}