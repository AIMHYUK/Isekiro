// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/JumpState_Strike.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"

UJumpState_Strike::UJumpState_Strike()
{
	StateDistance.Min = 0.f;
	StateDistance.Max = 650.f;
	MaxRunTime = .9f;
	TotalRunTime = 0.f;
	TravelDist = 500.f;
	Count = 0;
}

void UJumpState_Strike::Start()
{
	Super::Start();
}

EBossState UJumpState_Strike::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (FSMComp && !FSMComp->IsCurrentStateActive())
	{
		return FSMComp->RandomState();
	}
	return EBossState::NONE;
}

void UJumpState_Strike::Stop()
{
	if (Instigator)
	{
		Instigator->SetLockOnTarget(true);
	}
	return Super::Stop();
}

void UJumpState_Strike::StartMovement()
{
	Super::StartMovement();
	switch (Count)
	{
	case 0:
		PrevLoc = Instigator->GetActorLocation();
		NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::FORWARD);
		Count++;
		break;
	case 1:
		if (Target)
		{
			TotalRunTime = 0.f;
			MaxRunTime = .25f;
			PrevLoc = Instigator->GetActorLocation();
			NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::FORWARD);
			Count++;
		}
	default:
		break;
	}
}

EBossState UJumpState_Strike::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement()) return EBossState::NONE;

	if (TotalRunTime < MaxRunTime)
	{
		TotalRunTime += DeltaTime;

		FVector MoveVec = FMath::Lerp(PrevLoc, NewLoc, TotalRunTime / MaxRunTime);
		Instigator->SetActorLocation(MoveVec);
	}

	return EBossState::NONE;
}
