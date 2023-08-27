// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "Weapons/Weapon.h"
#include "BossExplosionSkill.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ABossExplosionSkill : public AWeapon
{
	GENERATED_BODY()
	ABossExplosionSkill();
	virtual void BeginPlay() override;
	virtual void Attack();
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ExplosionSkill, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* ExplosionNiagara;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ExplosionSkill, meta = (AllowPrivateAccess = "true"))
	UMaterial* RangeDecalMaterial;
	
	FTimerHandle ExplodeTimerHandle;
	FDamageEvent DamageEvent;

	float ExplosionRange;
	float ExplosionDelayTime;
};
