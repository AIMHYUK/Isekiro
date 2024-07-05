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
public:

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
protected:

	virtual void BeginPlay() override;

private:
	float HPPercent;
	UCharacterWidget* mainUI;
	class UBossWidget* BossUI;


};
