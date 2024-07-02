// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DodgeAttackState.h"

UDodgeAttackState::UDodgeAttackState()
{
}

void UDodgeAttackState::Start()
{
	PlayMontage();
}

EBossState UDodgeAttackState::Update(float DeltaTime)
{
	return EBossState::NONE;
}

void UDodgeAttackState::Stop()
{
}
