// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssassinGameMode.h"
#include "AssassinCharacter.h"
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
