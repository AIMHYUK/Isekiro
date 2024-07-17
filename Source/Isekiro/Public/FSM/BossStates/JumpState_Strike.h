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
	
	virtual void StartMovement() override;
protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MaxRunTime;	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float JumpHeight;

	float TotalRunTime;
	FVector PrevLoc;
	FVector NewLoc;

	bool bCompletedJump;
	bool bIsReachingApex;
	float MaxHalfJumpTime;
	float TotalJumpRunTime;
	FVector PrevJumpLoc;
	FVector ApexJumpLoc;
	FVector EndJumpLoc;

	void CalculateJump(FVector Start, FVector End, float Height);
	void RunJump(float DeltaTime);

	int32 Count;
};
