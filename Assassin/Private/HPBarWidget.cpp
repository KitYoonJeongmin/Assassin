// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBarWidget.h"

#include "Components/ProgressBar.h"

void UHPBarWidget::SetHPBarPercent(float Percent)
{
	UE_LOG(LogTemp, Warning, TEXT("Current Health Point Percent: %f"), Percent);
	HPBar->SetPercent(Percent);
}
