// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangeComponent.generated.h"

UENUM(BlueprintType)	
enum class ERangeWeaponState : uint8
{
	E_Throwdagger = 0   UMETA(DisplayName = "Throwdagger"),	
	E_SmokeBomb UMETA(DisplayName = "SmokeBomb"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeRangeWeaponStateDelegate,ERangeWeaponState,CurrentRangeWeaponState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API URangeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintCallable, Category = "Event")
	FOnChangeRangeWeaponStateDelegate OnChangeRangeWeaponState;
public:	
	// Sets default values for this component's properties
	URangeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ZoomIn();
	void ZoomOut();
	void Shoot();

	void ChangeWeapon(const FInputActionValue& Value);
	void SetCurrentRangeWeaponState(ERangeWeaponState ChangeState);
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class AAssassinCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Range, meta = (AllowPrivateAccess = "true"))
	class UProjectileBaseComponent* CurrentProjectileComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Range, meta = (AllowPrivateAccess = "true"))
	class UThrowDaggerComponent* ThrowDaggerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Range, meta = (AllowPrivateAccess = "true"))
	class USmokeBombComponent* SmokeBombComponent;

private:

	//SpringArm Location
	const FVector ZoomInLocation = FVector(0.f,60.f,-4.5f);
	const FVector ZoomOutLocation = FVector(0.f,0.f,0.f);
	FVector EndLocation;

	//Camera Component FOV
	float CurrentFOV;
	float TargetFOV;
	float ZoomOutFOV;
	const float ZoomInFOV = 30.f;;

	ERangeWeaponState CurrentRangeWeaponState;
};
