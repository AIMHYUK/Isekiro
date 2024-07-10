// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DeathState.h"
#include "Character/BossCharacter.h"

UDeathState::UDeathState()
{

}

void UDeathState::Start()
{
	Super::Start();

	if (Instigator)
	{
		if (FSMComp)
		{
			FSMComp->EnableStun(false);
		}
	}
}

EBossState UDeathState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (FSMComp && !FSMComp->IsCurrentStateActive())
	{
		return FSMComp->RandomState();
	}
	return EBossState::NONE;
}

void UDeathState::Stop()
{
	Super::Stop();
	FSMComp->EnableStun(true);
}
