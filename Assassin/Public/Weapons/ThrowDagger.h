// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileBase.h"
#include "ThrowDagger.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API AThrowDagger : public AProjectileBase
{
	GENERATED_BODY()
protected:
	AThrowDagger();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	virtual void ExcuteEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	virtual void Shoot(FVector TargetLoc);
private:
	const float HeadDamamge = 100.f;
	const float BodyDamage = 50.f;
};
