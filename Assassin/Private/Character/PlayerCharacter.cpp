// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "Character/Enemy/Enemy.h"
#include "Character/ACAnimInstance.h"

#include "Weapons/Weapon.h"
#include "Weapons/Sword.h"
#include "Weapons/Daggle.h"

#include "Camera/CameraComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


APlayerCharacter::APlayerCharacter()
{
    //Camera
    SequenceCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SequenceCamera"));
    SequenceCamera->SetupAttachment(RootComponent);
    FollowCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("FollowCameraChildActor"));
    FollowCameraChildActor->SetupAttachment(FollowCamera);
    SequenceCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("SequenceCameraChildActor"));
    SequenceCameraChildActor->SetupAttachment(SequenceCamera);
    //Health Point
    HealthPoint = 1000.f;

}
void APlayerCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    Weapon.DaggleWeapon = GetWorld()->SpawnActor<ADaggle>(FVector::ZeroVector, FRotator::ZeroRotator);
    AttachWeaponTo(Weapon.DaggleWeapon, FName("DaggleSocket"), false);
    Weapon.DaggleWeapon->InitializeWeapon();
    AttachWeaponTo(Weapon.DaggleWeapon, FName("DaggleHandSocket"), true);
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector RightDirectionX = GetInputRightDirection();
    FVector ForwardDirectionY = GetInputForwardDirection();
    FVector StartDebug = GetActorLocation() - GetActorUpVector() * 44.f;
    FVector EndDebug = StartDebug + RightDirectionX * MovementVector.X * 250.f + ForwardDirectionY * MovementVector.Y * 250.f;
    //DrawDebugDirectionalArrow(GetWorld(), StartDebug, EndDebug, 500.f, FColor::Magenta, false, -1.f, 0U, 2.5f);
    
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

TArray<AEnemy*> APlayerCharacter::DetectNearByEnemy(float SearchRadius)
{
    TArray<AEnemy*> NearbyEnemies;
    FVector CharacterLocation = GetActorLocation();
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    TArray<FHitResult> HitResults;
    UKismetSystemLibrary::SphereTraceMulti(GetWorld(), CharacterLocation, CharacterLocation, SearchRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);
    
    for (const FHitResult& HitResult : HitResults)
    {
        AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
        if (Enemy != nullptr && !Enemy->GetIsDead())
        {
            // 주변에 적이 검색되었으므로 처리
            NearbyEnemies.AddUnique(Enemy);
            
        }
    }

    return NearbyEnemies;
}

TArray<class AEnemy*> APlayerCharacter::DetectNearByEnemyInViewAngle(float SearchRadius, float FOVAngle)
{
    TArray<class AEnemy*> AngleEnemy;
    TArray<class AEnemy*> NearEnemy = DetectNearByEnemy(SearchRadius);
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
            AngleEnemy.Add(Enemy);
            
        }
    }

    return AngleEnemy;
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

AEnemy* APlayerCharacter::FindNearestEnemy(TArray<class AEnemy*> EnemyArr)
{
    AEnemy* NearestEnemyResult = nullptr;
    float MinDistance = TNumericLimits<float>::Max();

    for (AEnemy* Enemy : EnemyArr)
    {
        // 현재 위치와 거리 계산
        float Distance = FVector::Dist(Enemy->GetActorLocation(), GetActorLocation());

        // 가장 가까운 적인지 확인
        if (Distance < MinDistance)
        {
            NearestEnemyResult = Enemy;
            MinDistance = Distance;
        }
    }
    return NearestEnemyResult;
}
