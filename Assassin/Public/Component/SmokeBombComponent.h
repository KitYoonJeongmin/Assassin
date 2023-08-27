// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/ProjectileBaseComponent.h"
#include "SmokeBombComponent.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API USmokeBombComponent : public UProjectileBaseComponent
{
	GENERATED_BODY()
public:
	USmokeBombComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void DrawGuide();
	void DrawSpline();
	
	virtual void DeleteGuide() override;
	virtual void ShootProjectile();
	UFUNCTION()
	void GenerateSmokeBomb();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Spline, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* SplinePath;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Spline, meta = (AllowPrivateAccess = "true"))
	TArray<class USplineMeshComponent*> SplineMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Spline, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* SplineDefaultMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Spline, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CircleDecal;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Spline, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DefaultMaterial;
	
	FVector outVelocity;
	FVector startLoc;
	TSubclassOf<class ASmokeBomb> SmokeBombClass;
};
