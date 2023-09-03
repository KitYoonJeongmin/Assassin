// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EagleVisionComponent.h"

#include "Character/AssassinCharacter.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"

// Sets default values for this component's properties
UEagleVisionComponent::UEagleVisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CanEagleVision = false;
	EagleVisionRadius = 5000.f;
	CurrentEagleVisionRadius = 0.f;
	TargetEagleVisionRadius = 0.f;
}


// Called when the game starts
void UEagleVisionComponent::BeginPlay()
{
	Super::BeginPlay();
	TargetEagleVisionComps.Empty();
	Character = Cast<AAssassinCharacter>(GetOwner());
	DisableEagleVisionMat();
}

void UEagleVisionComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ExpandVisionRange();
	DetectTargets();
}

void UEagleVisionComponent::ExecuteEagleVision()
{
	if(CanEagleVision)
	{
		CanEagleVision = false;
		//Postprocess 백그라운드 비활성화
		if(EagleVisionPci == nullptr)
		{
			if(EagleVisionMat == nullptr) return;
			EagleVisionPci = GetWorld()->GetParameterCollectionInstance(EagleVisionMat);
		}
		TargetEagleVisionRadius = 0.f;
		EagleVisionPci->SetScalarParameterValue("EnableBackground", false);

		//객체 그린건 3초 뒤 비활성화
		GetWorld()->GetTimerManager().SetTimer(ColorDisableTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			float IsBackgroundEnable;
			EagleVisionPci->GetScalarParameterValue("EnableBackground",IsBackgroundEnable);
			if((bool)IsBackgroundEnable == false)
			{
				EagleVisionPci->SetScalarParameterValue("EnableColor", false);
			}
			
		}), ColorDisableWaitTime, false); 
	}
	else
	{
		CanEagleVision = true;
		//Postprocess활성화
		if(EagleVisionPci == nullptr)
		{
			if(EagleVisionMat == nullptr) return;
			EagleVisionPci = GetWorld()->GetParameterCollectionInstance(EagleVisionMat);
		}
		TargetEagleVisionRadius = EagleVisionRadius;
		EagleVisionPci->SetScalarParameterValue("EnableBackground", true);
		EagleVisionPci->SetScalarParameterValue("EnableColor", true);
	}
	
}

void UEagleVisionComponent::EnableEagleVisionMat()
{
	Character->GetMesh()->SetRenderCustomDepth(true);
	TArray<USceneComponent*> ChildComponents;
	Character->GetMesh()->GetChildrenComponents(true, ChildComponents);
	for(auto ChildMesh: ChildComponents)
	{
		USkeletalMeshComponent* SKMesh = Cast<USkeletalMeshComponent>(ChildMesh);
		if(SKMesh)
		{
			SKMesh->SetRenderCustomDepth(true);
		}
	}
}

void UEagleVisionComponent::DisableEagleVisionMat()
{
	Character->GetMesh()->SetRenderCustomDepth(false);
	TArray<USceneComponent*> ChildComponents;
	Character->GetMesh()->GetChildrenComponents(true, ChildComponents);
	for(auto ChildMesh: ChildComponents)
	{
		USkeletalMeshComponent* SKMesh = Cast<USkeletalMeshComponent>(ChildMesh);
		if(SKMesh)
		{
			SKMesh->SetRenderCustomDepth(false);
		}
	}
}



void UEagleVisionComponent::DetectTargets()
{
	if(!CanEagleVision) return;

	for(auto TargetEagleVisionComp: TargetEagleVisionComps)
	{
		TargetEagleVisionComp->DisableEagleVisionMat();
		TargetEagleVisionComps.Remove(TargetEagleVisionComp);
	}
	
	FVector StartLoc = Character->GetActorLocation();
	FVector EndLoc = StartLoc;

	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;
	if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLoc, EndLoc, CurrentEagleVisionRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		for(auto HitResult:HitResults)
		{
			IEagleVisionInterface* EagleVisionInterface = Cast<IEagleVisionInterface>(HitResult.GetActor());
			if(EagleVisionInterface)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *(HitResult.GetActor()->GetName()));
				if(!TargetEagleVisionComps.Contains(EagleVisionInterface->GetEagleVisionComponent()))
				{
					TargetEagleVisionComps.AddUnique(EagleVisionInterface->GetEagleVisionComponent());
					EagleVisionInterface->GetEagleVisionComponent()->EnableEagleVisionMat();
				}
			}
		}
	}
}

void UEagleVisionComponent::ExpandVisionRange()
{
	if(TargetEagleVisionRadius == EagleVisionRadius)
	{
		CurrentEagleVisionRadius = FMath::Lerp(CurrentEagleVisionRadius, TargetEagleVisionRadius+400.f, 0.01f);
	}
	else if(CurrentEagleVisionRadius != 0.f)
	{
		CurrentEagleVisionRadius = 0.f;
	}
	
	if(EagleVisionPci == nullptr)
	{
		if(EagleVisionMat == nullptr) return;
		EagleVisionPci = GetWorld()->GetParameterCollectionInstance(EagleVisionMat);
	}
	else
	{
		EagleVisionPci->SetScalarParameterValue("Radius", CurrentEagleVisionRadius);
	}
}



