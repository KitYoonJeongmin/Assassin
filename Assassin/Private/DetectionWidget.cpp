//// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionWidget.h"

#include "Components/ProgressBar.h"

void UDetectionWidget::SetDetectProgressValue(float DetectLevel)
{
	if(DetectProgress)
	{
		DetectProgress->SetPercent(DetectLevel);
	}
}
