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
#include "GameOverWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ShinobiExecutionWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	ExecutionWidget = CreateWidget<UShinobiExecutionWidget>(GetWorld(), ExecutionWidgetClass);
	//캐릭터가 죽으면 게임오버 위젯 + 3초후 게임 시작
}

void AIsekiroGameModeBase::Tick(float DeltaSeconds)
{
	AHeroCharacter* HeroCharacter = Cast<AHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsGameOverWidgetVisible && HeroCharacter->IsDead())
	{
		PlayerIsDead();
		IsGameOverWidgetVisible = true;
	}
}

void AIsekiroGameModeBase::GameHasEnded()
{
	BossUI->RemoveFromViewport();
	mainUI->RemoveFromViewport();
	if (ExecutionWidget) ExecutionWidget->AddToViewport();
	AHeroCharacter* HeroCharacter = Cast<AHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (HeroCharacter)
	{
		HeroCharacter->GetCharacterMovement()->DisableMovement();
		HeroCharacter->GetController()->SetIgnoreLookInput(true);
	}
}

void AIsekiroGameModeBase::PlayerIsDead()
{
	GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetFactory);
	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport(); //위젯띄우기
		UGameplayStatics::PlaySound2D(this, DeathSound);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AIsekiroGameModeBase::RestartLevel, 3.0f, false);	
	}
	
}

void AIsekiroGameModeBase::RestartLevel() //3초후 재시작
{
	FString MapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName));
}

UBossWidget* AIsekiroGameModeBase::GetBossUI()
{
	return BossUI;
}

void AIsekiroGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (BossWidget != nullptr)
	{
		BossUI = CreateWidget<UBossWidget>(GetWorld(), BossWidget);

		if (BossUI != nullptr)
			BossUI->AddToViewport();
	}
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

