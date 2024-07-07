// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/StrafeState.h"
#include "Character/BossCharacter.h"

UStrafeState::UStrafeState()
{
	StrafeSpeed = 100.f;
	StrafeMaxTime = 3.f;
	StrafeTotalTime = 0.f;
}

void UStrafeState::Start()
{
	Super::Start();
	StartMovement();
}

EBossState UStrafeState::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}

void UStrafeState::Stop()
{
	Super::Stop();
}

EBossState UStrafeState::UpdateMovement(float DeltaTime)
{
	if (StrafeTotalTime < StrafeMaxTime)
	{
		StrafeTotalTime += DeltaTime;

		if (Instigator)
		{
			FVector newLoc = Instigator->GetActorLocation() + Instigator->GetActorRightVector() * StrafeSpeed * DeltaTime;
			Instigator->SetActorLocation(newLoc);
		}
	}
	else {
		return EBossState::PARRY;
	}
	return EBossState::NONE;
}
