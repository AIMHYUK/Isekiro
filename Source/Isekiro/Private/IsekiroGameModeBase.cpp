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
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/AudioComponent.h"


AIsekiroGameModeBase::AIsekiroGameModeBase()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
}

void AIsekiroGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (LevelName == "MainMap") return;

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

	LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (LevelName == "GameWorld1")
	{

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
	}

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

void AIsekiroGameModeBase::PlayBackgroundAudio(EBackgroundAudioType Type)
{
	if (AudioComp)
	{
		auto AudioIt = BackgroundAudio.Find(Type);
		if (ensure(AudioIt && *AudioIt))
		{
			AudioComp->SetSound(*AudioIt);
			AudioComp->Play();
		}
	}
}

void AIsekiroGameModeBase::FadeInBackgroundAudio(EBackgroundAudioType Type, float FadeInTime)
{
	if (AudioComp)
	{
		auto AudioIt = BackgroundAudio.Find(Type);
		if (ensure(AudioIt && *AudioIt))
		{
			AudioComp->SetSound(*AudioIt);
			AudioComp->FadeIn(FadeInTime);
		}
	}
}

void AIsekiroGameModeBase::FadeOutBackgroundAudio(float FadeOutTime)
{
	if (AudioComp)
	{
		AudioComp->FadeOut(FadeOutTime, 0.f);
	}
}

UCharacterWidget* AIsekiroGameModeBase::GetMainUI()
{
	return mainUI;
}

void AIsekiroGameModeBase::GameHasEnded()
{
	if (ExecutionWidget) ExecutionWidget->AddToViewport();
	AHeroCharacter* HeroCharacter = Cast<AHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UGameplayStatics::PlaySound2D(GetWorld(), ExecutionSound);
	if (HeroCharacter)
	{
		HeroCharacter->GetCharacterMovement()->DisableMovement();
		HeroCharacter->GetController()->SetIgnoreLookInput(true);
	}
}

void AIsekiroGameModeBase::PlayerIsDead()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetFactory);
	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport(); //위젯띄우기
		UGameplayStatics::PlaySound2D(this, DeathSound);
		AudioComp->SetVolumeMultiplier(.65f);
	}
}

UBossWidget* AIsekiroGameModeBase::GetBossUI()
{
	return BossUI;
}

void AIsekiroGameModeBase::RemoveOneBossLife()
{
	GetBossUI()->RemoveBossLifePoint();
}

void AIsekiroGameModeBase::SpawnCollisionEffect(AActor* Owning, FVector Origin, EWeaponCollisionType Type)
{
	if (Owning)
	{
		auto GM = Owning->GetWorld()->GetAuthGameMode<AIsekiroGameModeBase>();
		if (GM) GM->SpawnWeaponCollisionEffect(Origin, Type);
	}
}

void AIsekiroGameModeBase::SpawnWeaponCollisionEffect(FVector Loc, EWeaponCollisionType Type)
{
	auto TypeRef = WeaponCollisionType.Find(Type);
	if (TypeRef && *TypeRef)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), *TypeRef, Loc);
	}
}

void AIsekiroGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (LevelName == "GameWorld1")
	{
		if (BossWidget != nullptr)
		{
			BossUI = CreateWidget<UBossWidget>(GetWorld(), BossWidget);

			if (BossUI != nullptr)
				BossUI->AddToViewport();
		}
	}	
}

void AIsekiroGameModeBase::UpdateHPBar()
{
	//if (mainUI)
	//{
	//	mainUI->HPBar->SetPercent(State->GetHPPercent());
	//}
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

