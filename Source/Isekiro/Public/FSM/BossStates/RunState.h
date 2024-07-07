// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "RunState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API URunState : public UStateObject
{
	GENERATED_BODY()
public:
	
	URunState();

	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
	
	virtual EBossState UpdateMovement(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TArray<EBossState> TransitionStates;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float RunSpeed;

private:
	EBossState SelectedState;
};
