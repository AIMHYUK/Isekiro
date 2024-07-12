// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DeathState.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"
#include "BossWidget.h"
#include "IsekiroGameModeBase.h"

UDeathState::UDeathState()
{
	bHasAlreadyResponded = false;
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
		Instigator->SetLockOnTarget(false);
		if (Instigator->GetBossUI()) Instigator->GetBossUI()->DisplayPostureBroken(true);
	}
}

EBossState UDeathState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (FSMComp && !FSMComp->IsCurrentStateActive())
	{
		auto Status = Instigator->GetComponentByClass<UStatusComponent>();
		if (Status && Status->IsAlive())
		{
			if (Instigator)
				Instigator->SetLockOnTarget(true);
			FSMComp->SetPostureState(EPostureState::STABLE);
			return FSMComp->RandomState();
		}
		else
		{
			Status->OfficiallyDeclareDead();
			auto GM = GetWorld()->GetAuthGameMode<AIsekiroGameModeBase>();
			if (GM)
				GM->GameHasEnded();
		}
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
	if (bHasAlreadyResponded) return;

	bHasAlreadyResponded = true;

	if (Instigator->GetBossUI()) Instigator->GetBossUI()->DisplayPostureBroken(false);

	Execute();
}

void UDeathState::Execute()
{
	auto Status = Instigator->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		if (Status->GetLifePoints() - 1 == 0)
		{
			JumpToSection("Execution");
		}
		else JumpToSection("RemoveLife");
	}
}