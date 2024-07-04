// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DistAttackState.h"

UDistAttackState::UDistAttackState()
{
    TriggerDistance = 1000.f;
}

void UDistAttackState::Start()
{
    Super::Start();
}

EBossState UDistAttackState::Update(float DeltaTime)
{
    return EBossState();
}

void UDistAttackState::Stop()
{
    Super::Stop();
}
