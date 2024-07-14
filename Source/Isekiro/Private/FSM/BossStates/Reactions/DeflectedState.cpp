// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/DeflectedState.h"
#include "Character/BossCharacter.h"

UDeflectedState::UDeflectedState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	StateDistance.Max = 350.f;
	TravelDist = 100.f;
}

void UDeflectedState::Start()
{
	Super::Start();

	//Child Functionality
	int32 SectionNum = FMath::RandRange(1, 2);
	FString Section = FString::FromInt(SectionNum);
	/*if (FMath::RandRange(0, 1) > 0)
	{
		Section += "_t"; //Completes Deflected State faster.
	}*/
	JumpToSection(FName(Section));

	if (FSMComp) FSMComp->EnableDefense(false);
}

void UDeflectedState::Stop()
{
	Super::Stop();
	if (FSMComp) FSMComp->EnableDefense(true);
}
