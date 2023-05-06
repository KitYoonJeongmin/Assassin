// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssassinCharacter.h"
#include "ACAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AAssassinCharacter

AAssassinCharacter::AAssassinCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	WalkSpeed = 300.f;
	RunSpeed = 600.f;
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	//Climbing Component
	CurrnetMovementState = EMovementState::E_Walking;
	ClimbingComp = CreateDefaultSubobject<UClimbingComponent>(TEXT("ClimbingComponent"));
}
void AAssassinCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//AnimInstance
	ACAnim = Cast<UACAnimInstance>(GetMesh()->GetAnimInstance());
	
}
void AAssassinCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	//Anim
	TArray<USceneComponent*> ChildrenMesh;
	GetMesh()->GetChildrenComponents(true, ChildrenMesh);
	for (auto mesh : ChildrenMesh)
	{
		if (Cast<USkeletalMeshComponent>(mesh))
			MeshArr.Add(Cast<USkeletalMeshComponent>(mesh));
	}
}

void AAssassinCharacter::Tick(float DeltaTime)
{
	
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_Walking:
		if (!isWalk)
		{
			UpdateMovementState(EMovementState::E_Walking);
			isWalk = true;
		}
		break;
	case MOVE_Falling:
		isWalk = false;
		break;
	}
	//const UEnum* CharStateEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMovementState"), true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, CharStateEnum->GetNameByValue((int64)ACAnim->CurrnetMovementState).ToString());
}



//////////////////////////////////////////////////////////////////////////
// Input

void AAssassinCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent != nullptr) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAssassinCharacter::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAssassinCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AAssassinCharacter::MoveEnd);
		
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAssassinCharacter::Look);

		//Runing
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AAssassinCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AAssassinCharacter::RunEnd);

		//Falling
		EnhancedInputComponent->BindAction(FallAction, ETriggerEvent::Started, this, &AAssassinCharacter::Fall);
	}

}

void AAssassinCharacter::Move(const FInputActionValue& Value)
{
	if (ClimbingComp->ClimbingState.CanMoveOnLedge) return;	//움직이는 중엔 축 변경 안함
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		switch (GetCharacterMovement()->MovementMode)
		{
		case MOVE_Walking:	//일반적으로 걸을 때
			AddMovementInput(RightDirection, MovementVector.X);
			AddMovementInput(ForwardDirection, MovementVector.Y);
			break;
		case MOVE_Flying:	//매달려 있을 때
			if (MovementVector.X != 0)	//좌우로 움직일 때
			{
				if (MovementVector.X > 0)
				{
					if (ClimbingComp->LedgeMoveRight())
					{
						ACAnim->SetCanMoveLedge(false, true);	//오른쪽으로 이동가능
					}
					else
					{
						ACAnim->SetCanMoveLedge(false, false);	//이동불가
						//ClimbingComp->FindLedge(MovementVector.X, MovementVector.Y);	//주변 Ledge찾음
					}
				}
				else
				{
					if (ClimbingComp->LedgeMoveLeft())
					{
						ACAnim->SetCanMoveLedge(true, false);	//왼쪽으로 이동가능
					}
					else
					{
						ACAnim->SetCanMoveLedge(false, false);	//이동불가
						//ClimbingComp->FindLedge(MovementVector.X, MovementVector.Y);	//주변 Ledge찾음
					}
				}
			}
			//if (MovementVector.Y != 0)
			//{
			//	
			//}
			break;
		}
		
	}
}

void AAssassinCharacter::MoveEnd(const FInputActionValue& Value)
{
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_Flying:	//매달려 있을 때 idle상태로 돌아감
		ACAnim->SetCanMoveLedge(false, false);
		break;
	}
}

void AAssassinCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAssassinCharacter::JumpStart()
{
	switch (CurrnetMovementState)
	{
	case EMovementState::E_Walking:
		UE_LOG(LogTemp, Warning, TEXT("00000"));
		//점프시 앞에 벽이 있고 잡을 수 있다면
		ClimbingComp->GetWallHeight();
		if (ClimbingComp->ClimbingState.CanClimbOnLedge == true)
		{
				ACAnim->PlayClimbingIdleMon();
		}
		else  //아니면 그냥 점프
		{
			Jump();
		}
		break;
	case EMovementState::E_Hanging:
		
		ClimbingComp->ClimbUp();
		if (MovementVector.Y >= 0&& ClimbingComp->ClimbingState.ClimbUpLedge)//매달린 상태에서 올라갈 수 있다면
		{
			//DisableInput(Cast<APlayerController>(GetController()));
			for (auto mesh : MeshArr)
			{
				Cast<UACAnimInstance>(mesh->GetAnimInstance())->UpdateMovementState(EMovementState::E_Walking);
				Cast<UACAnimInstance>(mesh->GetAnimInstance())->PlayClimbUpMon();
			}
			ACAnim->UpdateMovementState(EMovementState::E_Walking);
			ACAnim->PlayClimbUpMon();
			return;
		}
		else if(!ClimbingComp->ClimbingState.CanMoveOnLedge)
		{
			if (MovementVector.Y == 0)
			{
				if (MovementVector.X > 0 && ClimbingComp->LedgeMoveRight()) break;	//오른쪽 이동 가능하면 점프못함
				if (MovementVector.X < 0 && ClimbingComp->LedgeMoveLeft()) break;	//왼쪽 이동가능하면 점프못함
			}
			ClimbingComp->FindLedge(MovementVector.X, MovementVector.Y);	//주변 Ledge찾음
		}
		
		break;
	case EMovementState::E_Falling:
		break;
	}
}

void AAssassinCharacter::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AAssassinCharacter::RunEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AAssassinCharacter::Fall()
{
	switch (CurrnetMovementState)
	{
	case EMovementState::E_Walking:
		ClimbingComp->DropToHang();
		break;
	case EMovementState::E_Hanging:
		for (auto mesh : MeshArr)
		{
			Cast<UACAnimInstance>(mesh->GetAnimInstance())->UpdateMovementState(EMovementState::E_Walking);
		}
		ClimbingComp->Fall();
		break;
	}


}

void AAssassinCharacter::UpdateMovementState(EMovementState CurrentState)
{
	CurrnetMovementState = CurrentState;
	ACAnim->UpdateMovementState(CurrnetMovementState);
}



