// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "ThrustState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UThrustState : public UStateObject
{
	GENERATED_BODY()
public:
	UThrustState();

	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;

	virtual void StartMovement() override;

protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly)
	float MaxRunTime;

private:
	FVector Direction;
	float Speed;

	float EaseOutSine(float x) {
		return FMath::Sin((x * PI) / 2.f);
	}
};
