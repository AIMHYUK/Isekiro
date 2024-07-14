// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DodgeState.h"
#include "Character/BossCharacter.h"

UDodgeState::UDodgeState()
{
	StateDistance.Max = 300.f;
}

void UDodgeState::Start()
{
	if (Instigator)
	{
		switch (Instigator->GetCurrentDirection())
		{
		case EDirection::FORWARD:
		{
			JumpToSection("Dodge_B");
			break;
		}
		case EDirection::BACK:
		{
			JumpToSection("Dodge_B");
			break;
		}
		case EDirection::LEFT:
		{
			JumpToSection("Dodge_L");
			break;
		}
		case EDirection::RIGHT:
		{
			JumpToSection("Dodge_R");
			break;
		}
		}		
	}


	Super::Start();
}

EBossState UDodgeState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
}

void UDodgeState::Stop()
{
	Super::Stop();
}
