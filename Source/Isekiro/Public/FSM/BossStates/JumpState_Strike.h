// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "JumpState_Strike.generated.h"

/**
 *
 */
UCLASS()
class ISEKIRO_API UJumpState_Strike : public UStateObject
{
	GENERATED_BODY()
public:
	UJumpState_Strike();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MaxRunTime;	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float TriggerDistance = 900.f;
	float TotalRunTime;
	bool bHasPrevLoc;
	FVector PrevLoc;
	float EaseOutSine(float x) {
		return FMath::Sin((x * PI) / 2.f);
	}
};
