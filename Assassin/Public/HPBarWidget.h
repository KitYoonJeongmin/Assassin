// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UHPBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**Percent(0~1)*/
	void SetHPBarPercent(float Percent);
private:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HPBar;
};
