// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/RunState.h"
#include "Character/BossCharacter.h"


URunState::URunState()
{
	TriggerDistance = 700.f;
	DistanceForTransition = 1000.f;
	RunSpeed = 300.f;
}

void URunState::Start()
{
	Super::Start();
	StartMovement();
}

EBossState URunState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (Instigator->GetDistanceToTargetOffset() <= DistanceForTransition)
	{
		StopMovement();
		if (ensure(StatesToTransition.IsValidIndex(0)))
			return StatesToTransition[0];
	}
	return EBossState::NONE;
}

void URunState::Stop()
{
	Super::Stop();
}

EBossState URunState::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement()) return EBossState::NONE;

	if (Instigator->GetDistanceToTarget() > DistanceForTransition)
	{
		FVector DirVector = Instigator->GetTargetOffsetLocation() - Instigator->GetActorLocation();
		DirVector.Normalize();

		FVector NewLoc = DirVector * DeltaTime * RunSpeed + Instigator->GetActorLocation();
		Instigator->SetActorLocation(NewLoc);
	}

	return EBossState::NONE;
}

