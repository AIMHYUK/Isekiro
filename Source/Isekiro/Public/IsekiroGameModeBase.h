// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IsekiroGameModeBase.generated.h"

/**
 * 
 */
class UCharacterWidget;

UCLASS()
class ISEKIRO_API AIsekiroGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	friend class ABossCharacter;
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterWidget> MainWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBossWidget> BossWidget;

	class UStatusComponent* State;
	UPROPERTY()
    class UStatusComponent* BossState;
	
	void UpdateHPBar();
	void UpdatePostureBar();
	UFUNCTION(BlueprintCallable)
	void SetMaxPortion();
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentPortion();

	bool bIsPlayerBossFight = false;

	void GameHasEnded();

protected:

	virtual void BeginPlay() override;

private:
	float HPPercent;
	UCharacterWidget* mainUI;
	class UBossWidget* BossUI;


};
