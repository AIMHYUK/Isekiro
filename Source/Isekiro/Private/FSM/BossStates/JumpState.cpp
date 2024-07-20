// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/JumpState.h"
#include "Character/BossCharacter.h"

UJumpState::UJumpState()
{
	TravelDist = 400.f;
	
	ToApexTime = .8f;
	AtApexTime = 1.2f;
	FromApexTime = 1.2f;

	MaxJumpTime = ToApexTime;
	TotalJumpTime = 0.f;

	JumpState = EJumpState::ToApex;

	StateDistance.Max = 600.f;
}

void UJumpState::Start()
{
	Super::Start();

	ToApexHeight = TravelDist;
	AtApexHeight = 10.f;

	if (Instigator)
	{
		OriginLoc = Instigator->GetActorLocation();
		StartLoc = OriginLoc;
		EndLoc = StartLoc + Instigator->GetActorUpVector() * ToApexHeight;
		MaxJumpTime = ToApexTime;
	}
}

EBossState UJumpState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (JumpState == EJumpState::Done) return EBossState::NONE;

	TotalJumpTime += DeltaTime;

	switch (JumpState)
	{
	case EJumpState::ToApex:
	{
		if (TotalJumpTime < MaxJumpTime)
		{
			FVector CurrLoc = FMath::Lerp(StartLoc, EndLoc, EaseOutSine(TotalJumpTime / MaxJumpTime));
			Instigator->SetActorLocation(CurrLoc);
		}
		else
		{
			Instigator->SetActorLocation(EndLoc);

			TotalJumpTime = 0.f;
			MaxJumpTime = AtApexTime;
			StartLoc = Instigator->GetActorLocation();
			EndLoc = StartLoc + Instigator->GetActorUpVector() * AtApexHeight;

			JumpState = EJumpState::AtApex;
		}
		break;
	}
	case EJumpState::AtApex:
	{
		if (TotalJumpTime < MaxJumpTime)
		{
			FVector CurrLoc = FMath::Lerp(StartLoc, EndLoc, TotalJumpTime / MaxJumpTime);
			Instigator->SetActorLocation(CurrLoc);
		}
		else
		{
			Instigator->SetActorLocation(EndLoc);

			TotalJumpTime = 0.f;
			MaxJumpTime = FromApexTime;
			StartLoc = Instigator->GetActorLocation();
			EndLoc = OriginLoc;

			JumpState = EJumpState::FromApex;
		}
		break;
	}
	case EJumpState::FromApex:
	{
		if (TotalJumpTime < MaxJumpTime)
		{
			FVector CurrLoc = FMath::Lerp(StartLoc, EndLoc, EaseInSine(TotalJumpTime / MaxJumpTime));
			Instigator->SetActorLocation(CurrLoc);
		}
		else
		{
			Instigator->SetActorLocation(EndLoc);

			JumpState = EJumpState::Done;
		}
		break;
	}
	}

	return EBossState::NONE;
}
