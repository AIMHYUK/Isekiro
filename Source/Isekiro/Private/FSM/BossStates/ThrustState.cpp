// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/ThrustState.h"
#include "Character/BossCharacter.h"

UThrustState::UThrustState()
{
	MaxRunTime = .4f;
	TotalRunTime = .0f;

	TriggerDistance = 600.f;
	bIsWithinRange = true;
	Speed = 300.f;
}

void UThrustState::Start()
{
	Super::Start();
}

EBossState UThrustState::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}

void UThrustState::Stop()
{
	Super::Stop();
	Instigator->SetLockOnTarget(true);
}

void UThrustState::StartMovement()
{
	Super::StartMovement();

	if (Instigator && Target)
	{
		Instigator->SetLockOnTarget(false);

		StartLoc = Instigator->GetActorLocation();
		EndLoc = Instigator->GetTargetOffsetLocation();

		Dir = EndLoc - StartLoc;
		Dir.Normalize();

		float Dist = FVector::Distance(StartLoc, EndLoc);
		bIsWithinRange = Dist <= TriggerDistance;
	}
}

EBossState UThrustState::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Test: %s"), CanStartMovement() ? TEXT("YES") : TEXT("NO")));
		return EBossState::NONE;
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Test: %s"), bIsWithinRange ? TEXT("Within Attack Range") : TEXT("Out of Attack Range")));

	if (bIsWithinRange)
	{
		if (TotalRunTime < MaxRunTime)
		{
			TotalRunTime += DeltaTime;
			FVector Loc = FMath::Lerp(StartLoc, EndLoc, EaseOutSine(TotalRunTime / MaxRunTime));
			Instigator->SetActorLocation(Loc);
		}
	}
	else
	{
		if (Instigator)
		{
			FVector MoveVector = Dir * Speed * DeltaTime + Instigator->GetActorLocation();
			Instigator->SetActorLocation(MoveVector);
		}
	}

	return EBossState::NONE;
}
