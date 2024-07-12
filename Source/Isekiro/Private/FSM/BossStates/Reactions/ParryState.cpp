// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/ParryState.h"
#include "Character/BossCharacter.h"

UParryState::UParryState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	StateDistance.Max = 350.f;
}

void UParryState::Start()
{
	Super::Start();
	//Child Functionality
	int32 SectionNum = FMath::RandRange(1,6);
	JumpToSection(FName(FString::FromInt(SectionNum)));
}

void UParryState::RespondToInput()
{
	Super::RespondToInput();

	if (!FSMComp) return;


	float val = FMath::RandRange(0.f, 1.f);
	if (val <= .7f) FSMComp->ChangeStateTo(EBossState::NORMALATTACK);

	val = FMath::RandRange(0.f, 1.f);
	if (val <= .2f) FSMComp->ChangeStateTo(EBossState::COUNTERATTACK);

	val = FMath::RandRange(0.f, 1.f);
	if (val <= .2f) FSMComp->ChangeStateTo(EBossState::DODGE);	
}
