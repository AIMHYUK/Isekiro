// Fill out your copyright notice in the Description page of Project Settings.


#include "IsekiroGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.h"
#include "ActorComponents/StatusComponent.h"
#include "Components/ProgressBar.h"

void AIsekiroGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget != nullptr)
	{
		mainUI = CreateWidget<UCharacterWidget>(GetWorld(), MainWidget);
		
		if (mainUI != nullptr)
			mainUI->AddToViewport();
	}

}