// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/StrafeState.h"
#include "Character/BossCharacter.h"

UStrafeState::UStrafeState()
{
	StrafeSpeed = 80.f;
	StrafeMaxTime = 3.f;
	StrafeTotalTime = 0.f;
	StateDistance.Min = 500.f;
}

void UStrafeState::Start()
{
	Super::Start();
	StartMovement();
	StrafeMaxTime =FMath::RandRange(1.5f, StrafeMaxTime);
}

EBossState UStrafeState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if(StrafeTotalTime >= StrafeMaxTime && FSMComp)
	{
		return FSMComp->RandomState();
	}
	return EBossState::NONE;
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
