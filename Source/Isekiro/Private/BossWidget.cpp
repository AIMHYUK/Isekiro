// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"

void UBossWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto Boss = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());
	if (Boss)
	{
		auto Status = Boss->GetComponentByClass<UStatusComponent>();
		if (Status) WidgetSwitcher->SetActiveWidgetIndex(Status->GetLifePoints()); //Boss's total life points
	}
	else WidgetSwitcher->SetActiveWidgetIndex(0); //Boss's total life points
}

void UBossWidget::RemoveBossLifePoint()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(WidgetSwitcher->ActiveWidgetIndex - 1);
	}
}
