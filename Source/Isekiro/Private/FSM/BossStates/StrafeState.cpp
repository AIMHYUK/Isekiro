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
}

EBossState UStrafeState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

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
		return EBossState::LUNGE;
	}

	return EBossState::NONE;
}

void UStrafeState::Stop()
{
	Super::Stop();
}

void UStrafeState::Activate()
{
}
