// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/AssassinCharacter.h"
#include "Character/ACAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "Component/FootIKComponent.h"
#include "Component/FinisherComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/Sword.h"
#include "Weapons/Daggle.h"

#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


//////////////////////////////////////////////////////////////////////////
// AAssassinCharacter

AAssassinCharacter::AAssassinCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SparkNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/AMainDirectory/SparkFX/NE_Sparkle_System.NE_Sparkle_System'"));
	if (SparkNiagara.Succeeded())
	{
		SparkN = SparkNiagara.Object;
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BloodNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/FXVillesBloodVFXPack/Particles/Niagara/Real/NS_Blood_Spurt_02.NS_Blood_Spurt_02'"));
	if (BloodNiagara.Succeeded())
	{
		BloodN = BloodNiagara.Object;
	}
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	WalkSpeed = 250.f;
	RunSpeed = 600.f;
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = TargetWalkSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->MinAnalogWalkSpeed = 50.f;
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
	
	//IK
	FootIKComp = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIK"));
	
	//Dead
	IsDead = false;

	//Assassination
	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

}
void AAssassinCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//AnimInstance
	ACAnim = Cast<UACAnimInstance>(GetMesh()->GetAnimInstance());
	ACAnim->OnMontageEnded.AddDynamic(this, &AAssassinCharacter::OnAttackMontageEnded);
	//Anim IK
	FootIKComp->m_pCharacter = this;

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

	TargetWalkSpeed = 250.f;

	//Character Weapons Spawn  Sword
	Weapon.SwordWeapon = GetWorld()->SpawnActor<ASword>(FVector::ZeroVector, FRotator::ZeroRotator);
	AttachWeaponTo(Weapon.SwordWeapon, FName("SwordSocket"), false);
	Weapon.SwordWeapon->InitializeWeapon();

	
}

void AAssassinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	
	//MoveSpeed
	float InterpSpeed = 0.5f;
	float Alpha = FMath::Clamp(DeltaTime / InterpSpeed, 0.0f, 1.0f);
	float NewWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, TargetWalkSpeed, Alpha);
	GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;

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
		
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AAssassinCharacter::RunStart);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AAssassinCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AAssassinCharacter::RunEnd);

		//Falling
		EnhancedInputComponent->BindAction(FallAction, ETriggerEvent::Started, this, &AAssassinCharacter::Fall);
		
		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AAssassinCharacter::Attack);

		//Block
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &AAssassinCharacter::BlockStart);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AAssassinCharacter::BlockEnd);

		//Equip
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AAssassinCharacter::EquipWeapon);

		//Parry
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &AAssassinCharacter::TryParry);
		
		
	}


}

void AAssassinCharacter::Move(const FInputActionValue& Value)
{
	if (CurrentWeapon&&CurrentWeapon->GetFinisherComponent()->IsFinishing) return;
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
					}
				}
			}
			break;
		}
		if (CurrentWeapon == Weapon.SwordWeapon&& !Weapon.SwordWeapon->GetIsAttacking())
		{
			FRotator TargetRot = Rotation;
			TargetRot.Pitch = GetActorRotation().Pitch;
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 7.0f));
		}
	}
	
}

void AAssassinCharacter::MoveEnd(const FInputActionValue& Value)
{
	MovementVector.X = 0;
	MovementVector.Y = 0;
	if (Value.Get<FVector2D>().X > 0)
	{
		MovementVector.X = 0;
	}
	if (Value.Get<FVector2D>().Y > 0)
	{
		MovementVector.Y = 0;
	}
	
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
	if (CurrentWeapon == Weapon.SwordWeapon)
	{
		ACAnim->PlaySwordRollMontage();
		return;
	}
	switch (CurrnetMovementState)
	{
	case EMovementState::E_Walking:
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

void AAssassinCharacter::RunStart()
{
	if (CurrentWeapon == Weapon.SwordWeapon)
	{
		EquipWeapon();
		ACAnim->WeaponState = EWeaponState::E_Sword;
	}
}

void AAssassinCharacter::Run()
{
	if (CurrentWeapon != Weapon.SwordWeapon)
	{
		TargetWalkSpeed = RunSpeed;
	}
	else
	{
		TargetWalkSpeed = WalkSpeed;
	}
	
}

void AAssassinCharacter::RunEnd()
{
	TargetWalkSpeed = WalkSpeed;
}

void AAssassinCharacter::Fall()
{
	switch (CurrnetMovementState)
	{
	case EMovementState::E_Walking:
		ClimbingComp->DropToHang();
		break;
	case EMovementState::E_Hanging:
		ACAnim->UpdateMovementState(EMovementState::E_Walking);
		ClimbingComp->Fall();
		break;
	}


}

void AAssassinCharacter::SetWalkSpeed(float TargetSpeed)
{
	TargetWalkSpeed = TargetSpeed;
}

void AAssassinCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName().Contains("Hit")&& !bInterrupted)
	{
		OnStunEnd.Broadcast();
	}
	if (Montage->GetName().Contains("PAtt"))
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->GetFinisherComponent()->IsFinishing = false;
		}
	}

}

void AAssassinCharacter::UpdateMovementState(EMovementState CurrentState)
{
	CurrnetMovementState = CurrentState;
	ACAnim->UpdateMovementState(CurrnetMovementState);
}


float AAssassinCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentWeapon != Weapon.SwordWeapon)
	{
		AttachWeaponTo(Weapon.DaggleWeapon, FName("DaggleSocket"), false); //단검은 다른곳에 붙이기
		AttachWeaponTo(Weapon.SwordWeapon, FName("Sword_rSocket"), true);	//칼 잡기
	}

	//Push back
	FVector DamagedFrom = EventInstigator->GetPawn() ? EventInstigator->GetPawn()->GetActorLocation() : GetActorLocation();
	FVector DamageDirection = DamagedFrom - GetActorLocation(); 
	DamageDirection.Z = 0.f;
	DamageDirection.Normalize();
	DamageDirection *= -1;
	FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), EventInstigator->GetPawn()->GetActorLocation());
	SetActorRotation(NewRot);
	//Parrying 상태라면 무시
	if (CurrentWeapon == Weapon.SwordWeapon && Weapon.SwordWeapon->IsParry > 0)
	{
		AAssassinCharacter* Enemy = Cast<AAssassinCharacter>(EventInstigator->GetPawn());
		Enemy->ACAnim->PlaySwordHitMontage();
		Weapon.SwordWeapon->PlayParry();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SparkN, GetActorLocation(), DamageCauser->GetActorRotation() + FRotator(0.f, 0.f, -90.f));
		return 0.f;
	}


	//MontagePlay
	ACAnim->PlaySwordHitMontage();
	if (CurrentWeapon == Weapon.SwordWeapon && Weapon.SwordWeapon->GetIsBlock()) //막고 있는 상태일 때
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SparkN, GetActorLocation(), DamageCauser->GetActorRotation()+FRotator(0.f,0.f,-90.f));
		LaunchCharacter(DamageDirection, true, true);
	}
	else	//막고있지 않을 때
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodN, GetActorLocation(), DamageCauser->GetActorRotation() + FRotator(0.f, 0.f, -90.f));
		HealthPoint -= DamageAmount;
	}
	

	//Death
	if (HealthPoint <= 0.f)
	{
		ACAnim->PlaySwordDeathMontage();
	}
	
	return FinalDamage;
}

void AAssassinCharacter::BlockStart()
{
	if (CurrentWeapon == Weapon.SwordWeapon)
	{
		Weapon.SwordWeapon->EnableBlock();
	}
}

void AAssassinCharacter::BlockEnd()
{
	if (CurrentWeapon == Weapon.SwordWeapon)
	{
		Weapon.SwordWeapon->DisableBlock();
	}
}

void AAssassinCharacter::Attack()
{
	if (CurrentWeapon == nullptr) return;
	CurrentWeapon->Attack();
}

void AAssassinCharacter::TryParry()
{
	if (CurrentWeapon == Weapon.SwordWeapon)
	{
		Weapon.SwordWeapon->TryParry();
	}
}

void AAssassinCharacter::EquipWeapon()
{
	if (CurrentWeapon == Weapon.SwordWeapon)	//현재칼을 착용하고 있다면
	{
		ACAnim->PlayEquipMontage(false);	//칼 넣기
		
	}
	else  //착용하지 않았다면
	{
		ACAnim->PlayEquipMontage(true);		//착용
		
	}
	
}

void AAssassinCharacter::AttachWeaponTo(AWeapon* SwitchingWeapon, FName WeaponSocket, bool isEquip)
{
	if (nullptr == SwitchingWeapon) return;
	
	SwitchingWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocket);
	//SwitchingWeapon->SetOwner(this);

	if (isEquip)
	{
		CurrentWeapon = SwitchingWeapon;
		if (Cast<ASword>(SwitchingWeapon))
		{
			ACAnim->WeaponState = EWeaponState::E_Sword;
		}
		else if(Cast<ADaggle>(SwitchingWeapon))
		{
			ACAnim->WeaponState = EWeaponState::E_Daggle;
		}
		CurrentWeapon->IsUsed = true;
	}
	else
	{
		if(CurrentWeapon)
		{
			CurrentWeapon->IsUsed = false;
		}
		
		CurrentWeapon = nullptr;
		ACAnim->WeaponState = EWeaponState::E_Daggle;
	}
}

void AAssassinCharacter::Dead()
{
	IsDead = true;
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->Detach();
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
}

float AAssassinCharacter::GetHealthPoint()
{
	return HealthPoint;
}

void AAssassinCharacter::SetCustomDepth(bool IsEnable)
{
	GetMesh()->SetRenderCustomDepth(IsEnable);
	TArray<USceneComponent*> ChildComponents;
	GetMesh()->GetChildrenComponents(IsEnable, ChildComponents);
	for(auto ChildMesh: ChildComponents)
	{
		USkeletalMeshComponent* SKMesh = Cast<USkeletalMeshComponent>(ChildMesh);
		if(SKMesh)
		{
			SKMesh->SetRenderCustomDepth(IsEnable);
		}
	}
}


