// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/PatternState.h"
#include "Character/BossCharacter.h"

UPatternState::UPatternState()
{
	MaxRunTime = 1.2f;
	TotalRunTime = 0.f;
	counter = 0;
	TravelDist = 300.f;
	StateDistance.Min = 0.f;
	StateDistance.Max = 900.f;
}

void UPatternState::Start()
{
	Super::Start();
}

EBossState UPatternState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
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
		if(Instigator)
		{
			if(Instigator->IsWithinNearRange())
			{
				NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::BACK);	
			}
			else NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::FORWARD);
		}
		counter++;
		break;
	}
	case 1: //Forward roll and swinging sword
	{
		TravelDist = 500.f;
		NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::FORWARD);
		MaxRunTime = .9f;
		counter++;
		break;
	}
	case 2: 
	{
		TravelDist = 180.f;
		NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::FORWARD);
		MaxRunTime = .4f;
		break;
	}
	}
}

EBossState UPatternState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (TotalRunTime <= MaxRunTime)
	{
		TotalRunTime += DeltaTime;

		FVector Loc = FMath::Lerp(PrevLoc, NewLoc, EaseOutSine(TotalRunTime / MaxRunTime));

		Instigator->SetActorLocation(Loc);
	}

	return EBossState::NONE;
}
