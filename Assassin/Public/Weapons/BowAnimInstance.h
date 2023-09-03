// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BowAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UBowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	/**ArrowShoot*/
	void PlayDrawBowMontage();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bow, Meta = (AllowPrivateAccess = true))
	FVector StringTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bow, Meta = (AllowPrivateAccess = true))
	bool IsDrawing;
private:
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bow, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DrawBow;
};
