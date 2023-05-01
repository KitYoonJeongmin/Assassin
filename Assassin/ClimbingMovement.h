// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ClimbingMovement.generated.h"

enum class EMovementState : uint8;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UClimbingMovement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASSASSIN_API IClimbingMovement
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void UpdateMovementState(EMovementState MovementState) = 0;
};
