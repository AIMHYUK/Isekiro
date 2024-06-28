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
	if (Instigator && StateMontage)
	{
		Instigator->GetMesh()->GetAnimInstance()->Montage_Play(StateMontage);
	}
}

EBossState ULungeState::Update(float DeltaTime)
{
	EBossState State = Super::Update(DeltaTime);

	return State;
}

void ULungeState::Stop()
{
}

void ULungeState::Activate()
{
}

EBossState ULungeState::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Test: %s"), CanStartMovement() ? TEXT("YES") : TEXT("NO")));
		return EBossState::NONE;
	}

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
