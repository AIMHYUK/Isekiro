// Fill out your copyright notice in the Description page of Project Settings.


#include "IsekiroGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.h"
#include "ActorComponents/StatusComponent.h"
#include "GameFramework/Pawn.h"
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
}


void AIsekiroGameModeBase::UpdateHPBar()
{
	if (mainUI)
	{
        mainUI->HPBar->SetPercent(State->GetHPPercent());
        UE_LOG(LogTemp, Display, TEXT("MAUI"));
	}
}

void AIsekiroGameModeBase::UpdatePostureBar()
{
    if (mainUI)
    {
        mainUI->PostureBar->SetPercent(State->GetHPPercent());
        UE_LOG(LogTemp, Display, TEXT("MAUI"));
    }
}
