// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ACPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API AACPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AACPlayerController();
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;
	UFUNCTION()
	void OnEnableInput();
	UFUNCTION()
	void OnDisableInput();
};
