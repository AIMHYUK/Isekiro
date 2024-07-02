// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "LungeState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API ULungeState : public UStateObject
{
	GENERATED_BODY()

public:
	ULungeState();

	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
	virtual void Activate();

protected:
	bool bHasPrevLoc;
	FVector PrevLoc;
	float MaxRunTime = 2.f;
	float TotalRunTime;
	float JumpHeight;
	float JumpMaxTime;
	float JumpTotalTime;

	UPROPERTY(EditDefaultsOnly, Category = "Test")
	float TargetOffset;
private:

	FVector GetTargetOffsetLocation() const;

	float EaseInOutCubic(float x) {
		return x < 0.5 ? 4 * x * x * x : 1 - FMath::Pow(-2 * x + 2, 3) / 2;
	}
	float EaseOutSine(float x) {
		return FMath::Sin((x * PI) / 2.f);
	}
	float EaseOutQuart(float x) {
		return 1 - FMath::Pow(1 - x, 4);
	}
	float EaseOutCirc(float x) {
		return FMath::Sqrt(1 - FMath::Pow(x - 1, 2));
	}
};