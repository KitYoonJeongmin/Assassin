// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TimeDilation.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UAnimNotify_TimeDilation : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notify")
    float TimeDilation = 0.5f;
};
