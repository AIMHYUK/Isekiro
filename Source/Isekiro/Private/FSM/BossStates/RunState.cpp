// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/RunState.h"
#include "Character/BossCharacter.h"


URunState::URunState()
{
	StateDistance.Min = 850.f;
	RunSpeed = 500.f;
}

void URunState::Start()
{
	Super::Start();
	StartMovement();

	if (FSMComp && FSMComp->HasEnteredFight()) FSMComp->PlayBossSound(EBossDialogue::START02);
	SelectNextState();
}

EBossState URunState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	float DistToTarget = Instigator->GetDistanceToTargetOffset();
	if (DistToTarget <= StateDistance.Min)
	{
		if (FSMComp && FSMComp->TargetWithinRangeFor(SelectedState))
		{
			StopMovement();
			return SelectedState;
		}
		else
		{
			if (FSMComp && FSMComp->HasEnteredFight()) 
			{
				SelectedState = EBossState::PATTERNATTACK;
			}
			else SelectNextState();
		}
	}
	return EBossState::NONE;
}

void URunState::Stop()
{
	Super::Stop();
}

EBossState URunState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);
	float DistToTarget = Instigator->GetDistanceToTargetOffset();

	FVector DirVector = Instigator->GetTargetOffsetLocation() - Instigator->GetActorLocation();
	DirVector.Normalize();

	FVector NewLoc = DirVector * DeltaTime * RunSpeed + Instigator->GetActorLocation();
	Instigator->SetActorLocation(NewLoc);

	return EBossState::NONE;
}

void URunState::SelectNextState()
{
	if (FSMComp && FSMComp->HasEnteredFight())
	{
		SelectedState = EBossState::PATTERNATTACK;
	}
	else 
	{
		int32 index = FMath::RandRange(0, TransitionStates.Num() - 1);
		if (ensure(TransitionStates.IsValidIndex(index)))
			SelectedState = TransitionStates[index];
		else SelectedState = EBossState::NONE;
	}	
}

