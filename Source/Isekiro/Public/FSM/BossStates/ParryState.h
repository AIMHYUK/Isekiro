// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "ParryState.generated.h"

/**
 *
 */
UCLASS()
class ISEKIRO_API UParryState : public UStateObject
{
	GENERATED_BODY()
public:
	UParryState();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;

protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MaxRunTime;

	float TotalRunTime;
	FVector PrevLoc;

	FVector NewLoc;
	float EaseOutExpo(float x) {
		return x == 1 ? 1 : 1 - FMath::Pow(2, -10 * x);
	}
};
