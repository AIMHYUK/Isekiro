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

class UNiagaraSystem; 

UCLASS()
class ISEKIRO_API AIsekiroGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	friend class ABossCharacter;
public:
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
	void RestartLevel();
	UBossWidget* GetBossUI();
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditDefaultsOnly)
	USoundBase* ExecutionSound;

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
