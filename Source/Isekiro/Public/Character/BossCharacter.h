// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "BossCharacter.generated.h"

/**
 * 
 */

UENUM()
enum class ETestState : uint8 
{
	Strafe, 
	Run,
	Rush,
	Lunge
};

class UFSMComponent;

UCLASS()
class ISEKIRO_API ABossCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ABossCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	TObjectPtr<UFSMComponent> FSMComponent;
	
	UPROPERTY(EditInstanceOnly, Category = "Test")
	TObjectPtr<AActor> Target;
	UPROPERTY(EditInstanceOnly, Category = "Test")
	float BufferDist;

	UPROPERTY(EditInstanceOnly, Category = "Test")
	float StrafeSpeed;
	UPROPERTY(EditInstanceOnly, Category = "Test")
	float RunSpeed;

	UPROPERTY(EditInstanceOnly, Category = "Test")
	ETestState CurrentState;
	UPROPERTY(EditInstanceOnly, Category = "Test")
	float StateMaxTime;
	float TotalTime;

private:
	float GetDistanceToTarget() const;
};
