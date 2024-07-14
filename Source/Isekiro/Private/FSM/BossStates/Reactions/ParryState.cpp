// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/ParryState.h"
#include "Character/BossCharacter.h"
#include "HeroCharacter.h"

UParryState::UParryState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	StateDistance.Max = 350.f;
	TravelDist = 100.f;
}

void UParryState::Start()
{
	Super::Start();
	//Child Functionality
	int32 SectionNum = FMath::RandRange(1,6);
	JumpToSection(FName(FString::FromInt(SectionNum)));

	auto Hero = Cast<AHeroCharacter>(Target);
	if (Hero)
	{
		Hero->PlayParriedMontage();
	}
}
