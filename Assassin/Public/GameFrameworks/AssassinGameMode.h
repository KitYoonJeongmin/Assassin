// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AssassinGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	E_NormalMode = 0 UMETA(DisplayName = "NormalMode"),
	E_CombatMode = 1 UMETA(DisplayName = "CombatMode"),
};

UCLASS(minimalapi)
class AAssassinGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAssassinGameMode();
	virtual void BeginPlay() override;
	void ChangeGameModeToCombat();
	void ChangeGameModeToNormal();
	void CountCombatEnemyNum(bool IsIncrease);
private:
	EGameMode CurrentGameMode;
	int32 CombatEnemyNum;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> MyHUDClass;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = true))
	class UHUDWidget* HUDWidget;
	
	
};



