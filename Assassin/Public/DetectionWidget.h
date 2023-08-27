// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UDetectionWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* DetectProgress;
public:
	void SetDetectProgressValue(float DetectLevel);
};
