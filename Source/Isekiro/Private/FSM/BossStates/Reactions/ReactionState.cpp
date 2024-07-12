// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/ReactionState.h"
#include "Character/BossCharacter.h"

UReactionState::UReactionState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	StateDistance.Max = 350.f;

	TravelDist = 250.f;

	MeleeAttackProb = .8f;
	NormalAttackProb = .8f;
}

void UReactionState::Start()
{
	Super::Start();

	if (Instigator)
	{
		Instigator->SetLockOnTarget(true);
	}

	PrevLoc = Instigator->GetActorLocation();
	FVector TargetLoc = Instigator->GetTargetLoc();

	float Distance = FVector::Distance(PrevLoc, TargetLoc);
	FVector DirVec = PrevLoc - TargetLoc;
	DirVec.Normalize();

	if (Distance < Instigator->GetTargetOffset())
		NewLoc = Instigator->GetTargetOffset() * DirVec + TargetLoc;
	else
		NewLoc = Distance * DirVec + TargetLoc;

	NewLoc = TravelDist * DirVec + TargetLoc;
}

EBossState UReactionState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (FSMComp && !FSMComp->IsCurrentStateActive())
	{
		if (FMath::RandRange(0.f, 1.f) <= MeleeAttackProb)
		{
			if (FMath::RandRange(0.f, 1.f) <= NormalAttackProb)
			{
				return EBossState::NORMALATTACK;
			}
			else return EBossState::COUNTERATTACK;
		}
		else
		{
			return FSMComp->RandomState();
		}
	}
	return EBossState::NONE;
}

void UReactionState::Stop()
{
	Super::Stop();
}

EBossState UReactionState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (!CanStartMovement()) return EBossState::NONE;

	if (TotalRunTime <= MaxRunTime)
	{
		TotalRunTime += DeltaTime;

		FVector Loc = FMath::Lerp(PrevLoc, NewLoc, EaseOutExpo(TotalRunTime / MaxRunTime));

		Instigator->SetActorLocation(Loc);
	}

	return EBossState::NONE;
}
