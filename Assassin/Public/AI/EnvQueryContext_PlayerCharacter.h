// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_BlueprintBase.h"
#include "EnvQueryContext_PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UEnvQueryContext_PlayerCharacter : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
