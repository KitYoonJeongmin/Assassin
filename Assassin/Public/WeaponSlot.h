// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlot.generated.h"

/**
 * 
 */
UCLASS()
class ASSASSIN_API UWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void ChangeImage(ERangeWeaponState ChangeState);

private:
	UPROPERTY(meta=(BindWidget))
	class UImage* WeaponImg;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Image, Meta = (AllowPrivateAccess = true))
	class UTexture2D* SmokeImg;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Image, Meta = (AllowPrivateAccess = true))
	class UTexture2D* ThrowDaggerImg;
};
