// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DeathState.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"
#include "BossWidget.h"

UDeathState::UDeathState()
{
	bHasExecuted = false;
}

void UDeathState::Start()
{
	Super::Start();

	if (FSMComp)
	{
		FSMComp->EnableStun(false);
	}

	if (Instigator)
	{
		auto Status = Instigator->GetComponentByClass<UStatusComponent>();
		if (Status)
		{
			if (!Status->HasHealth())
			{
				Status->RemoveOneLifePoint();
				Status->IsAlive() ? JumpToSection("NoHealth") : JumpToSection("Death");
			}
			else if (Status->IsPostureBroken())
			{
				if (Instigator->GetBossUI()) Instigator->GetBossUI()->DisplayPostureBroken(true);
			}
		}
	}
}

EBossState UDeathState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (FSMComp && !FSMComp->IsCurrentStateActive())
	{
		return EBossState::NONE;
	}
	return EBossState::NONE;
}

void UDeathState::Stop()
{
	Super::Stop();
	FSMComp->EnableStun(true);
}

void UDeathState::RespondToInput()
{
	if (!FSMComp) return;
	if (!FSMComp->IsPostureBroken()) return;
	if (bHasExecuted) return;
	
	bHasExecuted = true;

	if (Instigator->GetBossUI()) Instigator->GetBossUI()->DisplayPostureBroken(false);

	Execute();
}

void UDeathState::Execute()
{
	auto Status = Instigator->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		if (Status->GetLifePoints() - 1 == 0 )
		{
			JumpToSection("Execution");
		}
		else JumpToSection("RemoveLife");
	}
}
