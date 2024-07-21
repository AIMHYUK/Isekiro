// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IsekiroGameModeBase.generated.h"

/**
 * 
 */
class UCharacterWidget;
class UGameOverWidget;
class UShinobiExecutionWidget;

UENUM()
enum class EWeaponCollisionType
{
	PARRY, 
	BLOCK, 
	DAMAGE
};

UENUM(Blueprintable)
enum class EBackgroundAudioType : uint8
{
	MAINMENU UMETA(DisplayName="MainMenu"),
	ASHINACASTLE UMETA(DisplayName = "AshinaCastle"),
	GENICHIRO UMETA(DisplayName = "Genichiro"),
	WIND UMETA(DisplayName = "Wind")
};

class UNiagaraSystem; 

UCLASS()
class ISEKIRO_API AIsekiroGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	friend class ABossCharacter;
public:
	AIsekiroGameModeBase();

	static void SpawnCollisionEffect(AActor* Initiated, FVector Origin, EWeaponCollisionType Type);

	UPROPERTY(EditDefaultsOnly)
	TMap<EWeaponCollisionType, UNiagaraSystem*> WeaponCollisionType;
	void SpawnWeaponCollisionEffect(FVector Loc, EWeaponCollisionType Type);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterWidget> MainWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBossWidget> BossWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameOverWidget> GameOverWidgetFactory;

	class UStatusComponent* State;
	UPROPERTY()
    class UStatusComponent* BossState;

	UPROPERTY(EditAnywhere)
	class USoundBase* DeathSound;

	void UpdateHPBar();
	void UpdatePostureBar();
	UFUNCTION(BlueprintCallable)
	void SetMaxPortion();
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentPortion();

	bool bIsPlayerBossFight = false;

	void GameHasEnded();

	void PlayerIsDead();
	UBossWidget* GetBossUI();
	UFUNCTION(BlueprintCallable)
	UCharacterWidget* GetMainUI();
	void RemoveOneBossLife();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditDefaultsOnly)
	USoundBase* ExecutionSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComp;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TMap<EBackgroundAudioType, USoundBase*> BackgroundAudio;
	
	UFUNCTION(BlueprintCallable)
	void PlayBackgroundAudio(EBackgroundAudioType Type);
	UFUNCTION(BlueprintCallable)
	void FadeInBackgroundAudio(EBackgroundAudioType Type, float FadeInTime);
	UFUNCTION(BlueprintCallable)
	void FadeOutBackgroundAudio(float FadeOutTime);

private:
	float HPPercent;
	UCharacterWidget* mainUI;
	class UBossWidget* BossUI;
	UGameOverWidget* GameOverWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UShinobiExecutionWidget> ExecutionWidgetClass;
	UPROPERTY()
	UShinobiExecutionWidget* ExecutionWidget;

	bool IsGameOverWidgetVisible = false;


};
