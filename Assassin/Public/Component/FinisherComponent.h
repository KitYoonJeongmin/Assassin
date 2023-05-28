// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FinisherComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UFinisherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFinisherComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//////Finish
public:
	void PlayFinish(class AAssassinCharacter* PlayerCharacter, class AAssassinCharacter* TargetCharacter);
	void MoveToActor(class AAssassinCharacter* PlayerCharacter, AActor* TargetActor, float Distance);
	void SwitchCamera(class APlayerCharacter* PlayerCharacter, bool UseSequence);
	bool IsFinishing;
		
};
