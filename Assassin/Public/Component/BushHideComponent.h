// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BushHideComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UBushHideComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBushHideComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void OnBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnableStealthMode();
	void DisableStealthMode();
private:
	int32 BushNum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BushDetectionBoxComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* SightBlockComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BushHide, meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Character;
};
