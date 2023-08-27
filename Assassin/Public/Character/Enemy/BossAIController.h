// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/AIControllerBase.h"
#include "BossAIController.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ABossAIController : public AAIControllerBase
{
	GENERATED_BODY()
public:
	ABossAIController();
	virtual void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);
	virtual void Sight(AActor* actor, FAIStimulus const Stimulus);
};
