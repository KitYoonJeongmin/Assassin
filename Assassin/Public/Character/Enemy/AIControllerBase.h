// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIControllerBase.generated.h"


UENUM(BlueprintType)
enum class EAIState : uint8
{
	E_Holding = 0 UMETA(DisplayName = "Holding"),
	E_Attacking = 1 UMETA(DisplayName = "Attacking"),
	E_Warning = 2 UMETA(DisplayName = "Warning"),
	E_Idle = 3 UMETA(DisplayName = "Idle"),
};
/**
 * 
 */
UCLASS()
class ASSASSIN_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
public:
	static const FName CanSeePlayerKey;
	static const FName TargetKey;
	static const FName IsStunKey;
	static const FName IsDeadKey;
	static const FName AIStateKey;
	
	AAIControllerBase();
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void Sight(AActor* actor, FAIStimulus const Stimulus) PURE_VIRTUAL(AAIControllerBase::Sight, );
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIAsset", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BTAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIAsset", meta = (AllowPrivateAccess = "true"))
	class UBlackboardData* BBAsset;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AISightRadius =1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AILoseSightRadius = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AIFieldOfView = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AISightAge = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AILastSeenLocation = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	class AEnemy* OwnerEnemy;
	EAIState CurrentState;
public:
	UFUNCTION()
	virtual void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus) PURE_VIRTUAL(AAIControllerBase::OnTargetDetected, );
	
	UFUNCTION()
	virtual void SetDead();
	UFUNCTION()
	void OnDisableBT();
	UFUNCTION()
	void OnEnableBT();
};
