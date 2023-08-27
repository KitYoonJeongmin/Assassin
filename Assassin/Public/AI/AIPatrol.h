// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrol.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UAIPatrol : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIPatrol();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FVector GetNextLocaiton();
	bool GetIsCheckPoint() const;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	class USplineComponent* PatrolPath;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	int32 CurrentPosition;

	float SplineLength; // spline 길이
	float DistanceOnSpline; //spline의 point를 구하는 거리
	float DistanceTerm; //Point간의 거리
	bool IsCheckPoint;
};
