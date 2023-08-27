// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "HPBarWidget.h"
#include "HUDWidget.h"
#include "Character/Enemy/Enemy.h"
#include "Character/ACAnimInstance.h"

#include "Weapons/Weapon.h"
#include "Weapons/Sword.h"
#include "Weapons/Dagger.h"

#include "Camera/CameraComponent.h"
#include "Component/BushHideComponent.h"
#include "Component/CoverMovementComponent.h"
#include "Component/EagleVisionComponent.h"
#include "Component/RangeComponent.h"
#include "Component/WhistleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameworks/AssassinGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/PlayerSword.h"


void APlayerCharacter::Move(const FInputActionValue& Value)
{
    switch (CurrnetMovementState)
    {
    case EMovementState::E_Covering:
        CoverMovementComponent->MoveInput(Value.Get<FVector2D>());
        break;
    case EMovementState::E_Stealthing:
        Super::Move(Value);
        SetWalkSpeed(100.f);
        break;
    default:
        SetWalkSpeed(300.f);
        Super::Move(Value);
        break;
    }
}

void APlayerCharacter::MoveEnd(const FInputActionValue& Value)
{
    Super::MoveEnd(Value);
    
    FVector2d ZeroVec;
    ZeroVec.Zero();
    CoverMovementComponent->MoveInput(ZeroVec);
}

APlayerCharacter::APlayerCharacter()
{
    //Camera
    SequenceCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SequenceCamera"));
    SequenceCamera->SetupAttachment(RootComponent);
    FollowCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("FollowCameraChildActor"));
    FollowCameraChildActor->SetupAttachment(FollowCamera);
    SequenceCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("SequenceCameraChildActor"));
    SequenceCameraChildActor->SetupAttachment(SequenceCamera);

    CoverMovementComponent = CreateDefaultSubobject<UCoverMovementComponent>(TEXT("CoverMovementComponent"));
    
    
    //Health Point
    HealthPoint = 1000.f;


    //BushHide
    BushHideComp = CreateDefaultSubobject<UBushHideComponent>(TEXT("BushHideComponent"));

    //WhistleComponent
    WhistleComponent = CreateDefaultSubobject<UWhistleComponent>(TEXT("WhistleComponent"));

    //Projectile
    SpringArmLoc = FVector(0.f,0.f,0.f);

    //Range
    RangeComponent = CreateDefaultSubobject<URangeComponent>(TEXT("RangeComponent"));

    // DetectType
    DetectEnemyTrace = UEngineTypes::ConvertToTraceType(ECC_Pawn);


    CurrentPlayerWeaponState = EPlayerWeaponState::E_Melee;
}
void APlayerCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();
    //Movement
    GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);

    //Character Weapons Spawn  Sword
    Weapon.SwordWeapon = GetWorld()->SpawnActor<APlayerSword>(FVector::ZeroVector, FRotator::ZeroRotator);
    AttachWeaponTo(Weapon.SwordWeapon, FName("SwordSocket"), false);
    Weapon.SwordWeapon->InitializeWeapon(this);
    
    Weapon.DaggleWeapon = GetWorld()->SpawnActor<ADagger>(FVector::ZeroVector, FRotator::ZeroRotator);
    AttachWeaponTo(Weapon.DaggleWeapon, FName("DaggleSocket"), false);
    Weapon.DaggleWeapon->InitializeWeapon(this);
    AttachWeaponTo(Weapon.DaggleWeapon, FName("DaggleHandSocket"), true);

    
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //CameraBoom->TargetOffset = FMath::FInterpTo(CameraBoom->TargetOffset, SpringArmLoc, DeltaTime,10.f);    //SpringArmLoc로 spring arm이 이동(나중에 SpringArmLoc 설정하는거 구현할것)
    
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent != nullptr)
    {
        //Cover
        EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Started, CoverMovementComponent , &UCoverMovementComponent::ToggleCoverMovement);
        //EagleVision
        EnhancedInputComponent->BindAction(EagleVisionAction, ETriggerEvent::Started, EagleVisionComponent , &UEagleVisionComponent::ExecuteEagleVision);
        //Whistle
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, WhistleComponent , &UWhistleComponent::ExecuteWhistle);
        //Zoom
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, RangeComponent , &URangeComponent::ZoomIn);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, RangeComponent , &URangeComponent::ZoomOut);
        //Shoot
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, RangeComponent , &URangeComponent::Shoot);
        //Projectile
        EnhancedInputComponent->BindAction(ChangeProjectile, ETriggerEvent::Started, RangeComponent , &URangeComponent::ChangeWeapon);
    }
    
}

FVector APlayerCharacter::GetInputRightDirection()
{
    FRotator MoveRot(0.f, GetController()->GetControlRotation().Yaw, 0.f);
    FVector RightDirectionX = FRotationMatrix(MoveRot).GetUnitAxis(EAxis::Y);
   
    return RightDirectionX;
}

FVector APlayerCharacter::GetInputForwardDirection()
{
    FRotator MoveRot(0.f, GetController()->GetControlRotation().Yaw, 0.f);
    FVector ForwardDirectionY = FRotationMatrix(MoveRot).GetUnitAxis(EAxis::X);

    return ForwardDirectionY;
}

TArray<class AEnemy*> APlayerCharacter::DetectNearByEnemyInViewAngle(float SearchRadius, float FOVAngle)
{
    TArray<class AEnemy*> AngleEnemy;
    TArray<AAssassinCharacter*> NearEnemy = DetectNearByEnemy(SearchRadius);
    FVector InputDirection = GetInputRightDirection()*MovementVector.X + GetInputForwardDirection() * MovementVector.Y;
    InputDirection.Normalize();

    for (auto Enemy : NearEnemy)
    {
        FVector EnemyDirection = (Enemy->GetActorLocation() - GetActorLocation());
        EnemyDirection.Normalize();
        float DotProduct = FVector::DotProduct(EnemyDirection, InputDirection);
        float MaxDotProduct = FMath::Cos(FMath::DegreesToRadians(FOVAngle / 2));    //시야각 계산

        if (DotProduct > MaxDotProduct) //시야각 내에 있는지 확인
        {
            AngleEnemy.Add(Cast<AEnemy>(Enemy));
            
        }
    }

    return AngleEnemy;
}

void APlayerCharacter::Attack()
{
    if(CurrentPlayerWeaponState != EPlayerWeaponState::E_Melee) return;
    Super::Attack();
}

void APlayerCharacter::AttachWeaponTo(class AWeapon* SwitchingWeapon, FName WeaponSocket, bool isEquip)
{
    Super::AttachWeaponTo(SwitchingWeapon, WeaponSocket, isEquip);
    if (isEquip)
    {
        if (Cast<ASword>(SwitchingWeapon))
        {
            GetCharacterMovement()->bOrientRotationToMovement = false;
        }
    }
    else
    {
        //bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        //GetCharacterMovement()->bUseControllerDesiredRotation = false;
    }
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser)
{
    float Result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Cast<AAssassinGameMode>(UGameplayStatics::GetGameMode(this))->HUDWidget->PlayerHPBar->SetHPBarPercent(HealthPoint/1000.f);
    
    return Result;
}

