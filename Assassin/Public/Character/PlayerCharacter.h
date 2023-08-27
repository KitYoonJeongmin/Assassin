// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AssassinCharacter.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerWeaponState : uint8
{
	E_Melee = 0 UMETA(DisplayName = "Melee"),
	E_Range = 1 UMETA(DisplayName = "Range"),
};


UCLASS()
class ASSASSIN_API APlayerCharacter : public AAssassinCharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CoverAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EagleVisionAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeProjectile;
	
	virtual void Move(const FInputActionValue& Value) override;
	virtual void MoveEnd(const FInputActionValue& Value) override;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Assassination, meta = (AllowPrivateAccess = "true"))
	class UCoverMovementComponent* CoverMovementComponent;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	FVector GetInputRightDirection();
	FVector GetInputForwardDirection();
public:
	
	/**시야각에 있는 적들을 찾는 함수*/
	TArray<class AEnemy*> DetectNearByEnemyInViewAngle(float SearchRadius, float FOVAngle);


	virtual void Attack() override;
	virtual void AttachWeaponTo(class AWeapon* SwitchingWeapon, FName WeaponSocket, bool isEquip) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
private:
	//식물속에 숨는 기능
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class UBushHideComponent* BushHideComp;

	//휘파람
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class UWhistleComponent* WhistleComponent;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	FVector SpringArmLoc;


	//Projectile
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Range, meta = (AllowPrivateAccess = "true"))
	class URangeComponent* RangeComponent;

	EPlayerWeaponState CurrentPlayerWeaponState;
	
};
