// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/LungeState.h"
#include "Character/BossCharacter.h"

ULungeState::ULungeState()
{
	bHasPrevLoc = false;
	PrevLoc = FVector::Zero();

	MaxRunTime = 2.f;
	TotalRunTime = 0.f;
	JumpHeight = 120.f;
	JumpTotalTime = 0.f;
}

void ULungeState::Start()
{
}

EBossState ULungeState::Update(float DeltaTime)
{
	if (!bHasPrevLoc)
	{
		bHasPrevLoc = true;
		PrevLoc = Instigator->GetActorLocation();
	}
	FVector lungeVector;
	FVector jumpVector;
	if (TotalRunTime < MaxRunTime) {
		TotalRunTime += DeltaTime;

		FVector ToTarget = Instigator->GetTargetOffsetLocation();
		ToTarget.Z = Instigator->GetActorLocation().Z;
		lungeVector = FMath::Lerp(PrevLoc, ToTarget, EaseOutSine(TotalRunTime / MaxRunTime));

		Instigator->SetActorLocation(lungeVector + jumpVector);
	}

	return EBossState::NONE;
}

void ULungeState::Stop()
{
}

void ULungeState::Activate()
{
}
