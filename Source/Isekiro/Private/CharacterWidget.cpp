// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidget.h"
#include "ActorComponents/StatusComponent.h"
#include "Components/ProgressBar.h"

void UCharacterWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StateComponent = GetOwningPlayerPawn()->GetComponentByClass<UStatusComponent>();
}

void UCharacterWidget::UpdateProgressBar()
{
    if (HPBar && StateComponent)
    {
        HPBar->SetPercent(StateComponent->GetHPPercent());
    }
}
