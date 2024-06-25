// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/LungeState.h"

ULungeState::ULungeState()
{
	TargetOffset = 150.f;

	bHasPrevLoc = false;
	PrevLoc = FVector::Zero();

	MaxRunTime = 2.f;
	TotalRunTime = 0.f;
	JumpHeight = 120.f;
	JumpMaxTime = .5f;
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

		FVector ToTarget = GetTargetOffsetLocation();
		ToTarget.Z = Instigator->GetActorLocation().Z;
		lungeVector = FMath::Lerp(PrevLoc, ToTarget, EaseOutSine(TotalRunTime / MaxRunTime));

		//if (JumpTotalTime < JumpMaxTime) {
		//	JumpTotalTime += DeltaTime;

		//	FVector jumpDest = GetActorUpVector() * JumpHeight;
		//	jumpVector = FMath::Lerp(PrevLoc, jumpDest, EaseOutSine(JumpTotalTime / JumpMaxTime));
		//}

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

FVector ULungeState::GetTargetOffsetLocation() const
{
	if (Target)
	{
		FVector WorkingVector = Target->GetActorForwardVector();
		WorkingVector *= TargetOffset;
		return WorkingVector;
	}
	return FVector();
}
