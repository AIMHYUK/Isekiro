// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DistAttackState.h"

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
