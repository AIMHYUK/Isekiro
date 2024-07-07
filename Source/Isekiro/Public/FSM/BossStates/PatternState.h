// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "PatternState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UPatternState : public UStateObject
{
	GENERATED_BODY()
	
public:
	UPatternState();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
	virtual void StartMovement() override;

protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MaxRunTime;	
	float TotalRunTime;
	FVector PrevLoc;

	int32 counter;

	FVector NewLoc;
	float EaseOutSine(float x) {
		return FMath::Sin((x * PI) / 2.f);
	}
};
