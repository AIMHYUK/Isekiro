// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "DodgeAttackState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UDodgeAttackState : public UStateObject
{
	GENERATED_BODY()
public:
	UDodgeAttackState();
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
};
