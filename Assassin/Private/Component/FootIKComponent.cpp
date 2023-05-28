// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FootIKComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UFootIKComponent::UFootIKComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFootIKComponent::BeginPlay()
{
	Super::BeginPlay();
    m_fIKCapsuleHalfHeight = m_pCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// ...
	
}


// Called every frame
void UFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    //Foot Rotator
    pTrace_Left = IKFootTrace(55.f, "foot_l");
    pTrace_Right = IKFootTrace(55.f, "foot_r");
    IK_Update_FootRotation(DeltaTime, NormalToRotator(pTrace_Left.pImpactLocation), &m_pFootRotation_Left, 13.f);
    IK_Update_FootRotation(DeltaTime, NormalToRotator(pTrace_Right.pImpactLocation), &m_pFootRotation_Right, 13.f);
   
    //Hip Location
    float fHipsOffset = UKismetMathLibrary::Min(pTrace_Left.fOffset, pTrace_Right.fOffset);
    if (fHipsOffset < 0.0f == false) fHipsOffset = 0.0f;
    IK_Update_FootOffset(DeltaTime, fHipsOffset, &m_fHipOffset, 13.0f);
    IK_Update_CapsuleHalfHeight(DeltaTime, fHipsOffset, false);

    //Foot Location
    IK_Update_FootOffset(DeltaTime, pTrace_Left.fOffset - fHipsOffset, &m_fFootOffsetLeft, 13.f);
    IK_Update_FootOffset(DeltaTime, (pTrace_Right.fOffset - fHipsOffset)*(-1), &m_fFootOffsetRight, 13.f);
}

ST_IK_TraceInfo UFootIKComponent::IKFootTrace(float fTraceDistance, FName sSocket)
{
   
    ST_IK_TraceInfo pTraceInfo;

    //! Set Linetraces startpoint and end point
    FVector pSocketLocation = m_pCharacter->GetMesh()->GetSocketLocation(sSocket);
    FVector pLine_Start = FVector(pSocketLocation.X, pSocketLocation.Y, m_pCharacter->GetActorLocation().Z);
    FVector pLine_End = FVector(pSocketLocation.X, pSocketLocation.Y
        , (m_pCharacter->GetActorLocation().Z - m_fIKCapsuleHalfHeight) - fTraceDistance);

    //! Process Line Trace
    FHitResult pHitResult;
    TArray<AActor*> pIgnore;
    pIgnore.Add(GetOwner());

    bool bDebug = false;
    EDrawDebugTrace::Type eDebug = EDrawDebugTrace::None;
    if (bDebug == true) eDebug = EDrawDebugTrace::ForOneFrame;

    bool bResult = UKismetSystemLibrary::LineTraceSingle(GetWorld(), pLine_Start, pLine_End,
        UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), true, pIgnore, eDebug, pHitResult, true);

    //! Set ImpactNormal and Offset from HitResult
    pTraceInfo.pImpactLocation = pHitResult.ImpactNormal;
    if (pHitResult.IsValidBlockingHit() == true)
    {
        float fImpactLegth = (pHitResult.ImpactPoint - pHitResult.TraceEnd).Size();
        pTraceInfo.fOffset = 5.0f + (fImpactLegth - fTraceDistance);
    }
    else
    {
        pTraceInfo.fOffset = 0.0f;
    }

    return pTraceInfo;
}

FRotator UFootIKComponent::NormalToRotator(FVector pVector)
{
    //IKFootTrace의 결과를 통해 바닥의 normal vector를 구함
    float fAtan2_1 = UKismetMathLibrary::DegAtan2(pVector.Y, pVector.Z);
    float fAtan2_2 = UKismetMathLibrary::DegAtan2(pVector.X, pVector.Z);
    fAtan2_2 *= -1.0f;
    FRotator pResult = FRotator(fAtan2_2, 0.0f, fAtan2_1);

    return pResult;
}

void UFootIKComponent::IK_Update_FootRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pFootRotatorValue, float fInterpSpeed)
{
    //NormalToRotator에서 나온 값을 보간하며 변하게 함
    FRotator pInterpRotator = UKismetMathLibrary::RInterpTo(*pFootRotatorValue, pTargetValue, fDeltaTime, fInterpSpeed);
    *pFootRotatorValue = pInterpRotator;
}

void UFootIKComponent::IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault)
{
    UCapsuleComponent* pCapsule = m_pCharacter->GetCapsuleComponent();
    if (pCapsule == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("IK : Capsule is NULL"));
        return;
    }

    //! Get Half Height of capsule component
    float fCapsuleHalf = 0.0f;
    if (bResetDefault == true)
    {
        fCapsuleHalf = m_fIKCapsuleHalfHeight;
    }
    else
    {
        float fHalfAbsSize = UKismetMathLibrary::Abs(fHipsShifts) * 0.5f;
        fCapsuleHalf = m_fIKCapsuleHalfHeight - fHalfAbsSize;
    }

    //! Set capsule component height with FInterpTo 
    float fScaledCapsuleHalfHeight = pCapsule->GetScaledCapsuleHalfHeight();
    float fInterpValue = UKismetMathLibrary::FInterpTo(fScaledCapsuleHalfHeight, fCapsuleHalf, fDeltaTime, 13.0f);

    pCapsule->SetCapsuleHalfHeight(fInterpValue, true);
}

void UFootIKComponent::IK_Update_FootOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float fInterpSpeed)
{
    float fInterpValue = UKismetMathLibrary::FInterpTo(*fEffectorValue, fTargetValue, fDeltaTime, fInterpSpeed);
    *fEffectorValue = fInterpValue;
}

