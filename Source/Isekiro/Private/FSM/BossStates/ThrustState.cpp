// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/ThrustState.h"
#include "Character/BossCharacter.h"

UThrustState::UThrustState()
{
	MaxRunTime = .5f;
	Speed = 2000.f;
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
}

void UThrustState::StartMovement()
{
	Super::StartMovement();

	if (Instigator && Target)
	{
		Direction = Target->GetActorLocation() -Instigator->GetActorLocation();
		Direction.Normalize();
	}
}

EBossState UThrustState::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Test: %s"), CanStartMovement() ? TEXT("YES") : TEXT("NO")));
		return EBossState::NONE;
	}
	FVector MoveVector = Instigator->GetActorLocation();
	MoveVector = (Direction * Speed * DeltaTime) + Instigator->GetActorLocation();
	Instigator->SetActorLocation(MoveVector);


	return EBossState::NONE;
}
