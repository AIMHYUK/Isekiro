// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "ReactionState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UReactionState : public UStateObject
{
	GENERATED_BODY()
public:
	UReactionState();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;

protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MaxRunTime;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta=(ClampMin="0.0"), meta=(ClampMax="1.0"))
	float MeleeAttackProb;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float NormalAttackProb;

	float TotalRunTime;
	FVector PrevLoc;
	FVector NewLoc;

	float EaseOutExpo(float x) {
		return x == 1 ? 1 : 1 - FMath::Pow(2, -10 * x);
	}


};
