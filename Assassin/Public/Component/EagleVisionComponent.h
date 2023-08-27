// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EagleVisionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UEagleVisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEagleVisionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ExecuteEagleVision();
	void EnableEagleVisionMat();
	void DisableEagleVisionMat();
	void DetectTargets();

	void ExpandVisionRange();
	
private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess))
	class AAssassinCharacter* Character;

	//포스트 프로세스 머티리얼
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PostProcess", Meta = (AllowPrivateAccess))
	UMaterialParameterCollection* EagleVisionMat;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PostProcess", Meta = (AllowPrivateAccess))
	class UMaterialParameterCollectionInstance* EagleVisionPci;

	//Targets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EagleVision", Meta = (AllowPrivateAccess))
	TArray<UEagleVisionComponent*> TargetEagleVisionComps;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EagleVision", Meta = (AllowPrivateAccess))
	bool CanEagleVision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EagleVision", Meta = (AllowPrivateAccess))
	float EagleVisionRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EagleVision", Meta = (AllowPrivateAccess))
	float CurrentEagleVisionRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EagleVision", Meta = (AllowPrivateAccess))
	float TargetEagleVisionRadius;

	//Timer Handle
	FTimerHandle ColorDisableTimerHandle;
	float ColorDisableWaitTime = 3.f; //시간을 설정하고
};
