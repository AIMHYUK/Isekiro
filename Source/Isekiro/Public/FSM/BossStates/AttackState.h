// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "AttackState.generated.h"

/**
 *
 */
UCLASS()
class ISEKIRO_API UAttackState : public UStateObject
{
	GENERATED_BODY()
public:
	UAttackState();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;

protected:
	virtual EBossState UpdateMovement(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MoveSpeed;

};
