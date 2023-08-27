// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/OutlineComponent.h"

#include "Character/AssassinCharacter.h"

// Sets default values for this component's properties
UOutlineComponent::UOutlineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	EnableScala = 0.5f;
	DisableScala = 0.f;
}


// Called when the game starts
void UOutlineComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AAssassinCharacter>(GetOwner());
	InitOverlayMaterialInstances();
	DisableOverlay();
}

void UOutlineComponent::EnableOverlay()
{
	if (OverlayMaterialInstances.Num()>0)
	{
		for(auto OverlayMat:OverlayMaterialInstances)
		{
			OverlayMat->SetScalarParameterValue("Scale", EnableScala);
		}
	}
}

void UOutlineComponent::DisableOverlay()
{
	if (OverlayMaterialInstances.Num()>0)
	{
		for(auto OverlayMat:OverlayMaterialInstances)
		{
			OverlayMat->SetScalarParameterValue("Scale", DisableScala);
		}
	}
}

void UOutlineComponent::InitOverlayMaterialInstances()
{
	TArray<USceneComponent*> MeshComponents;
	Character->GetComponents(MeshComponents,true);
	for(auto MeshComp: MeshComponents)
	{
		USkeletalMeshComponent* SKMesh = Cast<USkeletalMeshComponent>(MeshComp);
		if(SKMesh&&SKMesh->GetOverlayMaterial())
		{
			OverlayMaterialInstances.Add(UMaterialInstanceDynamic::Create(SKMesh->GetOverlayMaterial(), nullptr));
			SKMesh->SetOverlayMaterial(OverlayMaterialInstances[OverlayMaterialInstances.Num()-1]);
		}
	}
}

