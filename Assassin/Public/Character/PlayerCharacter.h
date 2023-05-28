// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AssassinCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API APlayerCharacter : public AAssassinCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SequenceCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* FollowCameraChildActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* SequenceCameraChildActor;
public:
	FVector GetInputRightDirection();
	FVector GetInputForwardDirection();
public:
	/**주변에 있는 적들을 찾는 함수*/
	TArray<class AEnemy*> DetectNearByEnemy(float SearchRadius);
	/**시야각에 있는 적들을 찾는 함수*/
	TArray<class AEnemy*> DetectNearByEnemyInViewAngle(float SearchRadius, float FOVAngle);
	/**가장 가까운 적을 찾는 함수*/
	class AEnemy* FindNearestEnemy(TArray<class AEnemy*> EnemyArr);

	virtual void AttachWeaponTo(class AWeapon* SwitchingWeapon, FName WeaponSocket, bool isEquip) override;
};
