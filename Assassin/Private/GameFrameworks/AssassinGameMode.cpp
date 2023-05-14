// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFrameworks/AssassinGameMode.h"
#include "Character/AssassinCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAssassinGameMode::AAssassinGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
