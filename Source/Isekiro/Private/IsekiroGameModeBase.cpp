// Fill out your copyright notice in the Description page of Project Settings.


#include "IsekiroGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.h"
#include "BossWidget.h"
#include "ActorComponents/StatusComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/ProgressBar.h"
#include "HeroCharacter.h"
#include "Character/BossCharacter.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void AIsekiroGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget != nullptr)
	{
		mainUI = CreateWidget<UCharacterWidget>(GetWorld(), MainWidget);

		if (mainUI != nullptr)
			mainUI->AddToViewport();
	}
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			State = PlayerPawn->FindComponentByClass<UStatusComponent>();
			if (State == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("StateComponent is null in AIsekiroGameModeBase::BeginPlay"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerPawn is null in AIsekiroGameModeBase::BeginPlay"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null in AIsekiroGameModeBase::BeginPlay"));
	}
	bIsPlayerBossFight = true;
	if (bIsPlayerBossFight)
	{
		if (BossWidget != nullptr)
		{
			BossUI = CreateWidget<UBossWidget>(GetWorld(), BossWidget);

			if (BossUI != nullptr)
				BossUI->AddToViewport();
		}
	}
}

void AIsekiroGameModeBase::GameHasEnded()
{


}



void AIsekiroGameModeBase::UpdateHPBar()
{
	if (mainUI)
	{
        mainUI->HPBar->SetPercent(State->GetHPPercent());
	}
}

void AIsekiroGameModeBase::UpdatePostureBar()
{
    if (mainUI)
    {
        mainUI->PostureBar->SetPercent(State->GetPosturePercent());
    }
}

void AIsekiroGameModeBase::SetMaxPortion()
{
	FString MaxPortion;
	MaxPortion = FString::Printf(TEXT("%d"), State->GetMaxPortion());
	if (mainUI)
	{
		mainUI->CurrentPortion->SetText(FText::FromString(MaxPortion));
	}
}

void AIsekiroGameModeBase::UpdateCurrentPortion()
{
	FString Portion;
	Portion = FString::Printf(TEXT("%d"), State->GetPortion());
	if (mainUI)
	{
		mainUI->CurrentPortion->SetText(FText::FromString(Portion));
	}
}

