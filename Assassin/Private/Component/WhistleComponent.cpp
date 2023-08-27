// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WhistleComponent.h"

#include "Character/AssassinCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

// Sets default values for this component's properties
UWhistleComponent::UWhistleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWhistleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWhistleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWhistleComponent::ExecuteWhistle()
{

	APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
	if(Character == nullptr) return;
	if(!(Character->CurrnetMovementState == EMovementState::E_Covering || Character->CurrnetMovementState == EMovementState::E_Stealthing)) return; //커버 스텔스 상태일때만 휘파람 가능
	UGameplayStatics::PlaySoundAtLocation(GetOwner(), WhistleSound, GetOwner()->GetActorLocation());
	// 노이즈 이벤트 생성
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		UAISense_Hearing::ReportNoiseEvent(GetOwner(), GetOwner()->GetActorLocation(), 1.0f, GetOwner(), 1000.f, FName("Whistle"));
	}), 1.1f, false); //반복도 여기서 추가 변수를 선언해 설정가능
	
}

