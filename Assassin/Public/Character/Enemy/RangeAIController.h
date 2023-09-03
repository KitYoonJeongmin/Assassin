// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeAIController.h"
#include "Character/Enemy/AIControllerBase.h"
#include "RangeAIController.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API ARangeAIController : public AMeleeAIController
{
	GENERATED_BODY()
	ARangeAIController();
public:
	virtual void ExcuteDetect() override;
};
