// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/BlockState.h"

UBlockState::UBlockState()
{
	TravelDist = 50.f;
}

void UBlockState::Start()
{
	Super::Start();
	int32 SectionNum = FMath::RandRange(1, 4);
	JumpToSection(FName(FString::FromInt(SectionNum)));
}
