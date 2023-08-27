// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/ProjectileBaseComponent.h"
#include "ThrowDaggerComponent.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UThrowDaggerComponent : public UProjectileBaseComponent
{
	GENERATED_BODY()
public:
	UThrowDaggerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DrawGuide();
	virtual void DeleteGuide();
	virtual void ShootProjectile();

	void AnimTrace();
	UFUNCTION()
	void GenerateDagger();
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> AimUIClass;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	class UThrowdaggerAnimWidget* AimUIObject;
private:
	TSubclassOf<class AThrowDagger> ThrowDaggerClass;
	FVector TargetLoc;
};
