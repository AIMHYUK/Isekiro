// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "DeathState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UDeathState : public UStateObject
{
	GENERATED_BODY()
public:
	UDeathState();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
protected:
	virtual void RespondToInput() override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USoundBase> OpeningVoiceLineSound;

private:
	UFUNCTION()
	void Execute();
	bool bHasAlreadyResponded;
};
