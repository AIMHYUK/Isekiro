// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
void UBossWidget::NativeConstruct()
{
	/*Super::NativeConstruct();*/
	Image_PostureBreak->SetVisibility(ESlateVisibility::Collapsed);
}

void UBossWidget::DisplayPostureBroken(bool bDisplay)
{
	if (bDisplay)
	{
		Image_PostureBreak->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Image_PostureBreak->SetVisibility(ESlateVisibility::Collapsed);
	}
}
