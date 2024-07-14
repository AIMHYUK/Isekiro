// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DistAttackState.h"

UDistAttackState::UDistAttackState()
{
    StateDistance.Min = 550.f;
}

void UDistAttackState::Start()
{
    Super::Start();
}

EBossState UDistAttackState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
}

void UDistAttackState::Stop()
{
    Super::Stop();
}
