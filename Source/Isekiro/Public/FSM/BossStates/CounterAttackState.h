// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/BossStates/AttackState.h"
#include "CounterAttackState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UCounterAttackState : public UAttackState
{
	GENERATED_BODY()
public:
	virtual EBossState Update(float DeltaTime) override;

};
