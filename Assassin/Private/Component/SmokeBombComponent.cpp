// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SmokeBombComponent.h"

#include "Camera/CameraComponent.h"
#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Weapons/SmokeBomb.h"

USmokeBombComponent::USmokeBombComponent()
{
	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SPLINEPATH"));
	CircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("DECALCOMPONENT"));
	//CircleDecal->SetDecalMaterial()
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BEAMMESH(TEXT("/Script/Engine.StaticMesh'/Game/AMainDirectory/Weapon/Materials/BeamMesh.BeamMesh'"));
	SplineDefaultMesh = BEAMMESH.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BEAMMATERIAL(TEXT("/Script/Engine.Material'/Game/AMainDirectory/Weapon/Materials/M_Path.M_Path'"));
	DefaultMaterial = BEAMMATERIAL.Object;

	static ConstructorHelpers::FObjectFinder<UBlueprint> BP_SmokeBomb(TEXT("/Script/Engine.Blueprint'/Game/AMainDirectory/Weapon/BP_SmokeBomb.BP_SmokeBomb'"));
	if (BP_SmokeBomb.Object)
	{
		SmokeBombClass = (UClass*)BP_SmokeBomb.Object->GeneratedClass;
	}
}

void USmokeBombComponent::BeginPlay()
{
	Super::BeginPlay();
	SplineDefaultMesh->SetMaterial(0,DefaultMaterial);

	OwnerCharacter->ACAnim->OnSpawnSmokeBomb.AddDynamic(this,&USmokeBombComponent::GenerateSmokeBomb);
}

void USmokeBombComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DrawSpline();
}

void USmokeBombComponent::DrawGuide()
{
	if(OwnerCharacter)
	{
		SplinePath->SetupAttachment(OwnerCharacter->GetRootComponent());
	}
	SetIsZooming(true);
}

void USmokeBombComponent::DrawSpline()
{
	
	/***********************이전 스플라인 정보 삭제*****************************/
	SplinePath->ClearSplinePoints(true);
	if(SplineMesh.Num() > 0)
	{
		for(int32 i=0; i<SplineMesh.Num(); i++)
		{
			if(SplineMesh[i])
			{
				SplineMesh[i]->DestroyComponent();
			}
		}
		SplineMesh.Empty();
	}
	
	/***********************************************************************/
 
	if(!IsZooming)
	{
		CircleDecal->SetVisibility(false);
		return;
	}
	
	startLoc = OwnerCharacter->GetActorLocation()+OwnerCharacter->GetControlRotation().Vector()*160.f+OwnerCharacter->GetActorUpVector()*50.f;      // 발사 지점
	FVector targetLoc = startLoc+OwnerCharacter->GetControlRotation().Vector()*1500.f+FVector(0.f,0.f,-1000.f);  // 타겟 지점.
	float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
	outVelocity = FVector::ZeroVector;   // 결과 Velocity
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
	{
		FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 15.0f, UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
		predictParams.DrawDebugTime = 15.0f;     //디버그 라인 보여지는 시간 (초)
		predictParams.DrawDebugType = EDrawDebugTrace::Type::None;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
		predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult result;
		
		const bool bSuccessPredictProjectilePath = UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		if(!bSuccessPredictProjectilePath) {return;}

		for(FPredictProjectilePathPointData point : result.PathData)
		{
			SplinePath->AddSplinePoint(point.Location,ESplineCoordinateSpace::World);
		}
		for(int32 SplineCount = 0; SplineCount<(SplinePath->GetNumberOfSplinePoints()-1); ++SplineCount)
		{
			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			
			SplineMeshComponent->SetStaticMesh(SplineDefaultMesh);
			SplineMeshComponent->SetMobility(EComponentMobility::Movable);
			SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SplineMeshComponent->AttachToComponent(SplinePath,FAttachmentTransformRules::KeepRelativeTransform);
			
			FVector StartPos = SplinePath->GetLocationAtSplinePoint(SplineCount,ESplineCoordinateSpace::Local);
			FVector StartTangent = SplinePath->GetTangentAtSplinePoint(SplineCount,ESplineCoordinateSpace::Local);
			FVector EndPos = SplinePath->GetLocationAtSplinePoint(SplineCount+1,ESplineCoordinateSpace::Local);
			FVector EndTangent = SplinePath->GetTangentAtSplinePoint(SplineCount+1,ESplineCoordinateSpace::Local);
			SplineMeshComponent->SetStartAndEnd(StartPos,StartTangent,EndPos,EndTangent);
			
			SplineMesh.Add(SplineMeshComponent);
		}
		CircleDecal->SetVisibility(true);
		CircleDecal->SetWorldLocation(result.LastTraceDestination.Location);
	}
}

void USmokeBombComponent::DeleteGuide()
{
	SetIsZooming(false);
}

void USmokeBombComponent::ShootProjectile()
{
	if(!IsZooming) return;
	FRotator newRot = OwnerCharacter->GetActorRotation();
	newRot.Yaw =  OwnerCharacter->GetControlRotation().Yaw;
	OwnerCharacter->SetActorRotation(newRot);
	OwnerCharacter->ACAnim->PlayThrowSmokeBombMontage();
}

void USmokeBombComponent::GenerateSmokeBomb()
{
	ASmokeBomb* SmokeBomb = GetWorld()->SpawnActor<ASmokeBomb>(SmokeBombClass,startLoc, OwnerCharacter->GetControlRotation());
	SmokeBomb->OwnerCharacter = OwnerCharacter;
	SmokeBomb->Shoot(outVelocity);
}
