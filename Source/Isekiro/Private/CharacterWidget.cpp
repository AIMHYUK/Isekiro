// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidget.h"
#include "ActorComponents/StatusComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "RealHpBar.h"


void UCharacterWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UCharacterWidget::UpdateProgressBar()
{
       
}

URealHpBar* UCharacterWidget::GetRealHpBar()
{
    return RealHpBar;
}
