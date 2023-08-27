// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThrowdaggerAnimWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UThrowdaggerAnimWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetAnimColor(bool IsWhite);
	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* AnimImg;


};
