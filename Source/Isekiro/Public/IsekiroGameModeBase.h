// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IsekiroGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API AIsekiroGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterWidget> MainWidget;

	class UStatusComponent* State;
	
	void UpdateHPBar();
	void UpdatePostureBar();
protected:

	virtual void BeginPlay() override;

private:
	float HPPercent;
	class UCharacterWidget* mainUI;


};
