// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DodgeState.h"
#include "Character/BossCharacter.h"

void UDodgeState::Start()
{
	if (Instigator)
	{
		switch (Instigator->GetCurrentDirection())
		{
		case EDirection::FORWARD:
		{
			MontageState.SectionName = "Dodge_B";
			break;
		}
		case EDirection::BACK:
		{
			MontageState.SectionName = "Dodge_B";
			break;
		}
		case EDirection::LEFT:
		{
			MontageState.SectionName = "Dodge_L";
			break;
		}
		case EDirection::RIGHT:
		{
			MontageState.SectionName = "Dodge_R";
			break;
		}
		}		
	}
	Super::Start();
}

EBossState UDodgeState::Update(float DeltaTime)
{
	EBossState State = Super::Update(DeltaTime);
	return State;
}

void UDodgeState::Stop()
{
	Super::Stop();
}
