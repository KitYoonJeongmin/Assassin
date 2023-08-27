// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OutlineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UOutlineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOutlineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// Overlay 머티리얼 활성화 함수
	UFUNCTION(BlueprintCallable, Category = "Overlay")
	void EnableOverlay();

	// Overlay 머티리얼 비활성화 함수
	UFUNCTION(BlueprintCallable, Category = "Overlay")
	void DisableOverlay();
	void InitOverlayMaterialInstances();
private:
	// 개별적인 Overlay 머티리얼 인스턴스
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Overlay", Meta = (AllowPrivateAccess))
	TArray<UMaterialInstanceDynamic*> OverlayMaterialInstances;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess))
	class AAssassinCharacter* Character;
	float EnableScala;
	float DisableScala;
};
