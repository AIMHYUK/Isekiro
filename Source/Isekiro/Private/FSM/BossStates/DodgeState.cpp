// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DodgeState.h"
#include "Character/BossCharacter.h"

void UDodgeState::Start()
{
	Super::Start();
	FName SectionName = "";
	if (Instigator)
	{
		switch (Instigator->GetCurrentDirection())
		{
		case EDirection::FORWARD:
		{
			SectionName = "Dodge_B";
			break;
		}
		case EDirection::BACK:
		{
			SectionName = "Dodge_B";
			break;
		}
		case EDirection::LEFT:
		{
			SectionName = "Dodge_L";
			break;
		}
		case EDirection::RIGHT:
		{
			SectionName = "Dodge_R";
			break;
		}
		}		
	}
	PlayMontage(SectionName);
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
