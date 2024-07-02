// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DodgeAttackState.h"

UDodgeAttackState::UDodgeAttackState()
{
}

void UDodgeAttackState::Start()
{
	Super::Start();
}

EBossState UDodgeAttackState::Update(float DeltaTime)
{
	return EBossState::NONE;
}

void UDodgeAttackState::Stop()
{
}
