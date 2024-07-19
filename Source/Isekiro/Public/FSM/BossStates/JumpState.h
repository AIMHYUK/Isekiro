// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "JumpState.generated.h"

UENUM()
enum class EJumpState
{
	ToApex,
	AtApex,
	FromApex,
	Done
};

UCLASS()
class ISEKIRO_API UJumpState : public UStateObject
{
	GENERATED_BODY()
public:
	UJumpState();
	virtual void Start() override;

	virtual EBossState UpdateMovement(float DeltaTime) override; 

protected:
	EJumpState JumpState;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Time")
	float ToApexTime;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Time")
	float AtApexTime;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Time")
	float FromApexTime;

private:
	float ToApexHeight;
	float AtApexHeight;

	float MaxJumpTime;
	float TotalJumpTime;

	FVector OriginLoc;
	FVector StartLoc;
	FVector EndLoc;

	float EaseOutSine(float x) {
		return FMath::Sin((x * PI) / 2);
	}
	float EaseInSine(float x) {
		return 1 - FMath::Cos((x * PI) / 2);
	}

};
