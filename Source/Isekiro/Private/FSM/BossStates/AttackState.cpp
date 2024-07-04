// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/AttackState.h"

void UAttackState::Start()
{
	Super::Start();
}

EBossState UAttackState::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}

void UAttackState::Stop()
{
	Super::Stop();
}

void UAttackState::StartMovement()
{
	Super::StartMovement(); 

	
}
