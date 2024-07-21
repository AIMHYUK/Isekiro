// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/JumpState_Strike.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"
#include "Components/CapsuleComponent.h"

UJumpState_Strike::UJumpState_Strike()
{
	StateDistance.Min = 0.f;
	StateDistance.Max = 650.f;
	MaxRunTime = .9f;
	TotalRunTime = 0.f;
	TravelDist = 500.f;

	TotalJumpRunTime = 0.f;
	JumpHeight = 300.f;
	bIsReachingApex = true;

	bCompletedJump = false;

	Count = 0;
}

void UJumpState_Strike::Start()
{
	Super::Start();
	MaxHalfJumpTime = MaxRunTime / 2.f;
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
	Instigator->ResetHeight();
	return Super::Stop();
}

void UJumpState_Strike::StartMovement()
{
	Super::StartMovement();
	switch (Count)
	{
	case 0:
	{
		PrevLoc = Instigator->GetActorLocation();
		NewLoc = Instigator->GetNewMovementLocation(TravelDist, EDirection::FORWARD);

		CalculateJump(PrevLoc, NewLoc, JumpHeight);

		Count++;
		break;
	}
	case 1:
		if (Instigator)
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
	Super::UpdateMovement(DeltaTime);

	switch (Count)
	{
	case 1:
	{
		UpdateJump(DeltaTime);
		break;
	}
	case 2:
		if (TotalRunTime < MaxRunTime)
		{
			TotalRunTime += DeltaTime;

			FVector MoveVec = FMath::Lerp(PrevLoc, NewLoc, TotalRunTime / MaxRunTime);

			Instigator->SetActorLocation(MoveVec);
		}
	default:
		break;
	}

	return EBossState::NONE;
}

void UJumpState_Strike::CalculateJump(FVector Start, FVector End, float Height)
{
	PrevJumpLoc = Start;
	EndJumpLoc = End;

	FVector DiffVec = End - Start;
	DiffVec /= 2.f;
	DiffVec += Start;
	DiffVec.Z += Height;
	ApexJumpLoc = DiffVec;
}

void UJumpState_Strike::UpdateJump(float DeltaTime)
{
	if (bCompletedJump) return;

	if (TotalJumpRunTime <= MaxHalfJumpTime)
	{
		TotalJumpRunTime += DeltaTime;

		if (bIsReachingApex)
		{
			FVector JumpVec = FMath::Lerp(PrevJumpLoc, ApexJumpLoc, EaseOutCubic(TotalJumpRunTime / MaxHalfJumpTime));

			Instigator->SetActorLocation(JumpVec);
		}
		else
		{
			FVector JumpVec = FMath::Lerp(ApexJumpLoc, EndJumpLoc, EaseInQuad(TotalJumpRunTime / MaxHalfJumpTime));

			Instigator->SetActorLocation(JumpVec);
		}
	}
	else
	{
		if (!bIsReachingApex)
		{
			Instigator->ResetHeight();
			bCompletedJump = true;
		}
		else
		{
			bIsReachingApex = false;
			TotalJumpRunTime = 0.f;
		}
	}
}
