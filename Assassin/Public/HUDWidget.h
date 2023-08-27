// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	class UWeaponSlot* ThrowWeaponSlot;
public:
	UPROPERTY(meta=(BindWidget))
	class UHPBarWidget* PlayerHPBar;
};
