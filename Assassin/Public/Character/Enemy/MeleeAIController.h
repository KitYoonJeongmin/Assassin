// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "MeleeAIController.generated.h"




/**
 * 
 */
UCLASS()
class ASSASSIN_API AMeleeAIController : public AAIControllerBase
{
	GENERATED_BODY()
public:
	static const FName CanHearSoundKey;
	static const FName TargetLocKey;
	static const FName IsAttackingKey;
	static const FName LastTargetLocKey;
public:
	AMeleeAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);

	virtual void Sight(AActor* actor, FAIStimulus const Stimulus);
	void Hearing(AActor* actor, FAIStimulus const Stimulus);
	void LoseSense();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIPerception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* HearingConfig;
	
	virtual void SetDead() override;
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackMontageStarted(UAnimMontage* Montage);
	//감지 관련 변수, 함수
public:
	UFUNCTION()
	void UpdateDetectionLevel();
	float GetDetectionLevel() const;
	virtual void ExcuteDetect();
private:
	bool IsIncreaseDetectionLevel;
	float DetectionLevel;
public:
	bool CanSense;

}; 
