// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbingMovement.h"
#include "ClimbingComponent.generated.h"


UENUM(BlueprintType)	
enum class EMovementState : uint8
{
	E_Walking = 0   UMETA(DisplayName = "Walking"),	//! DisplayName은 언리얼 에디터, 블루프린트에서 표시되는 이름입니다.
	E_OnLedge		UMETA(DisplayName = "OnLedge"),
	E_OnBeam		UMETA(DisplayName = "OnBeam"),
	E_Hanging		UMETA(DisplayName = "Hanging"),
	E_Jumping		UMETA(DisplayName = "Jumping"),
	E_Falling		UMETA(DisplayName = "Falling"),
	E_Flying		UMETA(DisplayName = "Flying"), 
	E_Mantelling	UMETA(DisplayName = "Mantelling"),
	E_Vault			UMETA(DisplayName = "Vault"),

};

USTRUCT(BlueprintType)
struct FClimbingState
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool CanClimbOnLedge;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool ClimbUpLedge;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool DropDown;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool HighMantle;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/**앞에 벽이 있는지 확인*/
	void GetWallLocation(FVector StartPoint);
	/**위에 잡을 수 있는지 확인*/
	void GetWallHeight();
	
	void GrabLedge();
	void SetOnWall(float OverTime);

public:
	UPROPERTY()
	FClimbingState ClimbingState;
private:
	class AAssassinCharacter* Character;
	class UCharacterMovementComponent* CharMoveComp;
	class UCapsuleComponent* CharCapsuleComp;
	class USkeletalMeshComponent* CharMeshComp;
	class UACAnimInstance* CharAnimInstance;

	
	class UArrowComponent* FrontWallTracer;	//올라갈 때
	class UArrowComponent* LeftMoveTracer;
	class UArrowComponent* RightMoveTracer;
	class UArrowComponent* ClimbUpTracer;

private:
	FVector WallImpactNormal;
	FVector WallImpactPoint;
	float WallHeightUpTraceLimit;
	float WallHeightDownTraceLimit;
	FRotator ClimbRotation;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LedgetLoc, Meta = (AllowPrivateAccess = true))
	float LedgeHightLocationXY;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LedgetLoc, Meta = (AllowPrivateAccess = true))
	float LedgeHightLocationZ;
	FVector LedgeLocation;
	FHitResult HightHitResult;
	FHitResult ForwardHitResult;
	//옆으로 이동
public:
	bool LedgeMoveLeft();
	bool LedgeMoveRight();
	void MoveOnLedge(FVector T1ImpactPoint, FVector T2ImpactPoint, FRotator Rot);

public:
	/**제일 위로 올라가 Walk상태로 돌리는 함수*/
	void ClimbUp();

public:
	/**난간에서 떨어짐*/
	void Fall();
};