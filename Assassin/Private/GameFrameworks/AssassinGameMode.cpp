// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFrameworks/AssassinGameMode.h"

#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Character/ACAnimInstance.h"
#include "Character/AssassinCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/Weapon.h"
#include "Weapons/Sword.h"

AAssassinGameMode::AAssassinGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	CurrentGameMode = EGameMode::E_NormalMode;
	CombatEnemyNum = 0;

	//HUD
	static ConstructorHelpers::FClassFinder<UUserWidget> MainWidgetAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/AMainDirectory/UI/UI_HUD.UI_HUD_C'"));
	if (MainWidgetAsset.Succeeded())
	{
		MyHUDClass = MainWidgetAsset.Class;
	}
	if (IsValid(MyHUDClass))
	{
		HUDWidget = Cast<UHUDWidget>(CreateWidget(GetWorld(), MyHUDClass));
	}
}

void AAssassinGameMode::BeginPlay()
{
	Super::BeginPlay();
	HUDWidget->AddToViewport();
}

void AAssassinGameMode::ChangeGameModeToCombat()
{
	CurrentGameMode = EGameMode::E_CombatMode;
	AAssassinCharacter* PlayerCharacter = Cast<AAssassinCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if(PlayerCharacter && PlayerCharacter->GetCharacterMovement()->MovementMode == MOVE_Walking
		&& PlayerCharacter->GetCurrentWeapon() != PlayerCharacter->Weapon.SwordWeapon )
	{
		PlayerCharacter->ACAnim->PlayEquipMontage(true);
	}
}

void AAssassinGameMode::ChangeGameModeToNormal()
{
	
	//게임모드를 normal로 변경
	CurrentGameMode = EGameMode::E_NormalMode;
	CombatEnemyNum = 0.f;

	// 무기 넣기
	AAssassinCharacter* PlayerCharacter = Cast<AAssassinCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if(PlayerCharacter && PlayerCharacter->GetCurrentWeapon() == PlayerCharacter->Weapon.SwordWeapon)
	{

		FTimerHandle WaitHandle;
		float WaitTime = 0.f;
		// 현재 실행 중인 몽타주 가져오기
		UAnimMontage* CurrentMontage = PlayerCharacter->ACAnim->GetCurrentActiveMontage();

		if (CurrentMontage)
		{
			// 현재 실행 중인 몽타주의 길이와 재생 시간 가져오기
			const float CurrentMontageLength = CurrentMontage->GetPlayLength();
			const float CurrentMontageTime = PlayerCharacter->ACAnim->Montage_GetPosition(CurrentMontage);
			WaitTime = CurrentMontageLength-CurrentMontageTime;
		}

		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			Cast<AAssassinCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0))->ACAnim->PlayEquipMontage(false);
		}), WaitTime, false);
		
	}
}

void AAssassinGameMode::CountCombatEnemyNum(bool IsIncrease)
{
	if(IsIncrease)
	{
		if(CurrentGameMode != EGameMode::E_CombatMode)
		{
			ChangeGameModeToCombat();
		}
		CombatEnemyNum++;
	}
	else
	{
		CombatEnemyNum--;
		if(CombatEnemyNum<=0)
		{
			ChangeGameModeToNormal();
		}
	}
	UE_LOG(LogTemp, Display, TEXT("GameMode.Sense Enemy Num:%d"), CombatEnemyNum);
}


