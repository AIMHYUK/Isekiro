// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/JumpState_Strike.h"
#include "Character/BossCharacter.h"

UJumpState_Strike::UJumpState_Strike()
{
	TriggerDistance = 150.f;
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

void UJumpState_Strike::StartMovement()
{
	Super::StartMovement();
	PrevLoc = Instigator->GetActorLocation();
	NewLoc = Instigator->GetNewMovementLocation(TravelDistance);
}

EBossState UJumpState_Strike::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Test: %s"), CanStartMovement() ? TEXT("YES") : TEXT("NO")));
		return EBossState::NONE;
	}
	
	if (TotalRunTime < MaxRunTime) 
	{		
		TotalRunTime += DeltaTime;

		FVector MoveVec = FMath::Lerp(PrevLoc, NewLoc, TotalRunTime / MaxRunTime);
		Instigator->SetActorLocation(MoveVec);	
	}

	return EBossState::NONE;
}
