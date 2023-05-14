// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootIKComponent.generated.h"

typedef struct ST_IK_TraceInfo
{
	float fOffset;
	FVector pImpactLocation;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASSASSIN_API UFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootIKComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	ST_IK_TraceInfo IKFootTrace(float fTraceDistance, FName sSocket);
	FRotator NormalToRotator(FVector pVector);
	void IK_Update_FootRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pFootRotatorValue, float fInterpSpeed);
	void IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault);
	void IK_Update_FootOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float fInterpSpeed);
public:
	ACharacter* m_pCharacter;
	FRotator m_pFootRotation_Left;
	FRotator m_pFootRotation_Right;
	float m_fHipOffset;
	float m_fFootOffsetLeft;
	float m_fFootOffsetRight;
private:
	ST_IK_TraceInfo pTrace_Left;
	ST_IK_TraceInfo pTrace_Right;
	float m_fIKCapsuleHalfHeight;

	
};
