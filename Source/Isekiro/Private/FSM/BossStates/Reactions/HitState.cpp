// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/HitState.h"
#include "Character/BossCharacter.h"

UHitState::UHitState()
{
	MaxRunTime = .6f;
	TotalRunTime = 0.f;
	TravelDist = 100.f;
	StateDistance.Max = 350.f;
}

void UHitState::Start()
{
	Super::Start();
	int32 SectionNum = FMath::RandRange(1, 4);
	JumpToSection(FName(FString::FromInt(SectionNum)));
}