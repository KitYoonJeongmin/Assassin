// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UProjectileBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void DrawGuide() PURE_VIRTUAL(UProjectileBaseComponent::DrawGuide, );
	virtual void DeleteGuide() PURE_VIRTUAL(UProjectileBaseComponent::DeleteGuide, );
	virtual void ShootProjectile() PURE_VIRTUAL(UProjectileBaseComponent::ShootProjectile, );

public:
	void SetOwnerCharacter(class AAssassinCharacter* OwnerChar) {OwnerCharacter = OwnerChar;}
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class AAssassinCharacter* OwnerCharacter;

	bool IsZooming;
	void SetIsZooming(bool ZoomState);
		
};
