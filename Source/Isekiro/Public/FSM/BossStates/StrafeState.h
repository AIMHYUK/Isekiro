// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "StrafeState.generated.h"

/**
 *
 */
UCLASS()
class ISEKIRO_API UStrafeState : public UStateObject
{
	GENERATED_BODY()
public:
	UStrafeState();

	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TArray<EBossState> TransitionStates;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float StrafeSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float StrafeMaxTime;
	float StrafeTotalTime;
private:
	UFUNCTION()
	void EnteredFight();
};
