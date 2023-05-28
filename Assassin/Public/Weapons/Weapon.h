// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class ASSASSIN_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION() /**공격을 시작할 때 동작*/
		virtual void Attack() PURE_VIRTUAL(AWeapon::Attack, );
	UFUNCTION() 
		virtual void InitializeWeapon();
	bool IsUsed;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Owner")
	class AAssassinCharacter* Character;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Finisher")
	class UFinisherComponent* FinisherComp;
	float WeaponDamage;
	bool IsAttacking;
public:
	void Detach();
	bool GetIsAttacking();
	UFinisherComponent* GetFinisherComponent();

};
