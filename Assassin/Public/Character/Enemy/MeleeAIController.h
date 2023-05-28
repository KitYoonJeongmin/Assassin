// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MeleeAIController.generated.h"


UENUM(BlueprintType)
enum class EAIState : uint8
{
	E_Holding = 0 UMETA(DisplayName = "Holding"),
	E_Attacking = 1 UMETA(DisplayName = "Attacking"),
	E_Recovery = 2 UMETA(DisplayName = "Recovery"),
};

/**
 * 
 */
UCLASS()
class ASSASSIN_API AMeleeAIController : public AAIController
{
	GENERATED_BODY()
public:
	static const FName CanSeePlayerKey;
	static const FName AIStateKey;
	static const FName TargetKey;
	static const FName TargetLocKey;
	static const FName IsStunKey;
	static const FName IsDeadKey;
	static const FName IsAttackingKey;
public:
	AMeleeAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);
	void Sight(AActor* actor, FAIStimulus const Stimulus);
	void Hearing(AActor* actor, FAIStimulus const Stimulus);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIAsset", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BTAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIAsset", meta = (AllowPrivateAccess = "true"))
	class UBlackboardData* BBAsset;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AISightRadius =1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AILoseSightRadius = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AIFieldOfView = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AISightAge = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	float AILastSeenLocation = 900.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIPerceptionSight", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIPerception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* HearingConfig;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	class AEnemy* OwnerEnemy;

	EAIState CurrentState;

public:
	UFUNCTION()
	void OnDisableBT();
	UFUNCTION()
	void OnEnableBT();
	UFUNCTION()
	void SetDead();
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackMontageStarted(UAnimMontage* Montage);

}; 
