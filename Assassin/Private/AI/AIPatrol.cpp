// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIPatrol.h"

#include "Components/SplineComponent.h"

// Sets default values for this component's properties
UAIPatrol::UAIPatrol()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DistanceOnSpline = 1.f;
	DistanceTerm = 600.f;
	IsCheckPoint = true;
}


// Called when the game starts
void UAIPatrol::BeginPlay()
{
	Super::BeginPlay();

	
	PatrolPath = GetOwner()->FindComponentByClass<USplineComponent>();
	PatrolPath->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	if(PatrolPath == nullptr)
	{
		//if(GEngine) GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,TEXT("Couldn't find patrol spline!"));
	}
	else
	{
		SplineLength = PatrolPath->GetSplineLength();
		//if(GEngine) GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,TEXT("Spline Length:  %f", SplineLength));
	}
	
}

void UAIPatrol::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

FVector UAIPatrol::GetNextLocaiton()
{
	//스플라인이 없는 경우
	if (PatrolPath == nullptr) 
	{
		IsCheckPoint = false;
		//if(GEngine) GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,TEXT("Couldn't find patrol spline!"));
		return GetOwner()->GetActorLocation();
	}

	//스플라인이 존재하는 경우
	if (DistanceOnSpline <= 0.f) //첫 부분 이라면
	{
		DistanceTerm *= -1; //추적 방향을 바꿔줌
		IsCheckPoint = true;//끝 부분 이니깐 멈춤
		UE_LOG(LogTemp, Warning, TEXT("%s:--------There is START point in patrol spline---------"),*(GetOwner()->GetName()));
		//if(GEngine) GEngine->AddOnScreenDebugMessage(-1,10,FColor::White,TEXT("There is START point in patrol spline"));
	}
	else if (DistanceOnSpline >= SplineLength)//끝 부분 이라면
	{
		DistanceTerm *= -1; //추적 방향을 바꿔줌
		IsCheckPoint = true;//끝 부분 이니깐 멈춤
		UE_LOG(LogTemp, Warning, TEXT("%s:--------There is END point in patrol spline---------"),*(GetOwner()->GetName()));
		//if(GEngine) GEngine->AddOnScreenDebugMessage(-1,10,FColor::White,TEXT("There is END point in patrol spline"));
	}
	else
	{
		IsCheckPoint = false;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Patrol"));
	}
	DistanceOnSpline += DistanceTerm;
	float DistanceAlongSpline = FMath::Clamp(DistanceOnSpline, 0.0f, SplineLength); // 길이를 유효한 범위로 제한
	return PatrolPath->GetLocationAtDistanceAlongSpline(DistanceOnSpline, ESplineCoordinateSpace::World);
}

bool UAIPatrol::GetIsCheckPoint() const
{
	return IsCheckPoint;
}

