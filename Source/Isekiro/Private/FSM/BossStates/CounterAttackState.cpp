// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/CounterAttackState.h"



UCounterAttackState::UCounterAttackState()
{
	StateDistance.Max = 420.f;
}

EBossState UCounterAttackState::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}