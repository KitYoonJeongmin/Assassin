// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowdaggerAnimWidget.h"

#include "Components/Image.h"

void UThrowdaggerAnimWidget::SetAnimColor(bool IsWhite)
{
	
	if(AnimImg)
	{
		FLinearColor NewColor;
		if(IsWhite)
		{
			NewColor = FLinearColor::White;
		}
		else
		{
			NewColor = FLinearColor::Red;
		}
		AnimImg->SetColorAndOpacity(NewColor);
	}
}
