// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Weapon.h"
#include "Arrow.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API AArrow : public AWeapon
{
	GENERATED_BODY()
	AArrow();
public:
	virtual void BeginPlay() override;
	virtual void Attack();
	
	FVector TargetLoc;
	
	UFUNCTION()   
	void OnOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	USkeletalMeshComponent* GetSkeletalMesh() const {return ArrowSkeletalMeshComponent;}
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SkeletalMesh")
	USkeletalMeshComponent* ArrowSkeletalMeshComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SkeletalMesh")
	UProjectileMovementComponent* ProjectileComp;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* SphereComponent;

};
