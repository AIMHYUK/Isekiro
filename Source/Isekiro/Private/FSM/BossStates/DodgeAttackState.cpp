// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DodgeAttackState.h"

UDodgeAttackState::UDodgeAttackState()
{
	StateDistance.Max = 200.f;
}

void UDodgeAttackState::Start()
{
	Super::Start();
}

EBossState UDodgeAttackState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
}

void UDodgeAttackState::Stop()
{
	Super::Stop();
}
