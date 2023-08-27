// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileBase.h"
#include "SmokeBomb.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ASmokeBomb : public AProjectileBase
{
	GENERATED_BODY()
	ASmokeBomb();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
public:
	virtual void ExcuteEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	virtual void Shoot(FVector TargetLoc);
	void StunEnemy();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Niagara, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* SightBlockComp;

	bool IsBomb;
	TArray<class AEnemy*> Enemys;
};
