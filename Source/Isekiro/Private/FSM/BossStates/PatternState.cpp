// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/PatternState.h"
#include "Character/BossCharacter.h"

UPatternState::UPatternState()
{
	MaxRunTime = 1.2f;
	TotalRunTime = 0.f;
	counter = 0;
	TravelDistance = 300.f;
	TriggerDistance = 500.f;
}

void UPatternState::Start()
{
	Super::Start();
}

EBossState UPatternState::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}

void UPatternState::Stop()
{
	Super::Stop();
}

void UPatternState::StartMovement()
{
	Super::StartMovement();

	PrevLoc = Instigator->GetActorLocation();
	TotalRunTime = 0.f;

	switch (counter)
	{
	case 0: // Fire Arrow
	{
		NewLoc = Instigator->GetNewMovementLocation(TravelDistance);
		counter++;
		break;
	}
	case 1: //Forward roll and swinging sword
	{
		TravelDistance = 500.f;
		NewLoc = Instigator->GetNewMovementLocation(TravelDistance);
		MaxRunTime = 1.45f;
		break;
	}
	}
}

EBossState UPatternState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (!CanStartMovement()) return EBossState::NONE;

	if (TotalRunTime <= MaxRunTime)
	{
		TotalRunTime += DeltaTime;

		FVector Loc = FMath::Lerp(PrevLoc, NewLoc, EaseOutSine(TotalRunTime / MaxRunTime));

		Instigator->SetActorLocation(Loc);
	}

	return EBossState::NONE;
}
