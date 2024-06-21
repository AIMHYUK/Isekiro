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
class UStateObject;

UCLASS()
class ISEKIRO_API ABossCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ABossCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	TObjectPtr<UFSMComponent> FSMComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TSubclassOf<UStateObject> StateObjClass;

	UPROPERTY(EditInstanceOnly, Category = "Test")
	TObjectPtr<AActor> TargetO;
	UPROPERTY(EditInstanceOnly, Category = "Test")
	float TargetOffset;
	
	bool bHasPrevLoc;
	FVector PrevLoc;
	float MaxRunTime = 2.f;
	float TotalRunTime;

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
	FVector GetTargetOffsetLocation() const;
		
	float EaseInOutCubic(float x) {
		return x < 0.5 ? 4 * x * x * x : 1 - FMath::Pow(-2 * x + 2, 3) / 2;
	}
	float EaseOutSine(float x){
		return FMath::Sin((x * PI) / 2.f);
	}	
	float EaseOutQuart(float x){
		return 1 - FMath::Pow(1 - x, 4);
	}		
	float EaseOutCirc(float x) {
		return FMath::Sqrt(1 - FMath::Pow(x - 1, 2));
	}
};
