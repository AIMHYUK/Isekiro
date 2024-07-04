// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/JumpState_Strike.h"
#include "Character/BossCharacter.h"

UJumpState_Strike::UJumpState_Strike()
{
	TriggerDistance = 150.f;
	PrevLoc = FVector::Zero();
	MaxRunTime = .9f;
	TotalRunTime = 0.f;	
}

void UJumpState_Strike::Start()
{
	Super::Start();
}

EBossState UJumpState_Strike::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}

void UJumpState_Strike::Stop()
{
	return Super::Stop();
}

EBossState UJumpState_Strike::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Test: %s"), CanStartMovement() ? TEXT("YES") : TEXT("NO")));
		return EBossState::NONE;
	}

	if (!bHasPrevLoc)
	{
		bHasPrevLoc = true;
		PrevLoc = Instigator->GetActorLocation();
	}

	FVector lungeVector = Instigator->GetActorLocation();
	FVector ToTarget = Instigator->GetTargetOffsetLocation();
	DrawDebugSphere(GetWorld(), ToTarget, 5.f, 12, FColor::Blue, true);
	if (TotalRunTime < MaxRunTime) {

		TotalRunTime += DeltaTime / 2.f;

		lungeVector = FMath::Lerp(PrevLoc, ToTarget, EaseOutSine(TotalRunTime / MaxRunTime));
	}

	Instigator->SetActorLocation(lungeVector);

	return EBossState::NONE;
}
