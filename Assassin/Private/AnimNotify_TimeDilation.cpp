// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_TimeDilation.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_TimeDilation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);

}