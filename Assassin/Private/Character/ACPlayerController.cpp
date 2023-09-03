// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACPlayerController.h"
#include "Character/AssassinCharacter.h"

AACPlayerController::AACPlayerController()
{

}

void AACPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AACPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	if (Cast<AAssassinCharacter>(aPawn))
	{
		Cast<AAssassinCharacter>(aPawn)->OnStunStart.AddDynamic(this, &AACPlayerController::OnDisableInput);
		Cast<AAssassinCharacter>(aPawn)->OnStunEnd.AddDynamic(this, &AACPlayerController::OnEnableInput);
	}
}

void AACPlayerController::OnEnableInput()
{
	
	GetPawn()->EnableInput(this);

}

void AACPlayerController::OnDisableInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Debug: PlayerControlelr is disable")));
	GetPawn()->DisableInput(this);
}
