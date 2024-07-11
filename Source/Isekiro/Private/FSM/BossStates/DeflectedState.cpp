// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/DeflectedState.h"
#include "Character/BossCharacter.h"

UDeflectedState::UDeflectedState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	StateDistance.Max = 350.f;
}

void UDeflectedState::Start()
{
	Super::Start();

	if (Instigator)
	{
		Instigator->SetLockOnTarget(true);
	}

	PrevLoc = Instigator->GetActorLocation();
	FVector TargetLoc = Instigator->GetTargetLoc();

	float Distance = FVector::Distance(PrevLoc, TargetLoc);
	FVector DirVec = PrevLoc - TargetLoc;
	DirVec.Normalize();

	if (Distance < Instigator->GetTargetOffset())
		NewLoc = Instigator->GetTargetOffset() * DirVec + TargetLoc;
	else
		NewLoc = Distance * DirVec + TargetLoc;

	NewLoc = 200.f * DirVec + TargetLoc;

	int32 SectionNum = FMath::RandRange(1, 2);
	FString Section = FString::FromInt(SectionNum);
	if (FMath::RandRange(0, 1) > 0)
	{
		Section += "_t";
	}
	JumpToSection(FName(Section));
}

EBossState UDeflectedState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (FMath::RandRange(1, 10) <= 8)
	{
		if (FMath::RandRange(1, 10) <= 7)
		{
			return EBossState::NORMALATTACK;
		}
		else return EBossState::COUNTERATTACK;
	}
	else
	{
		if (FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	}

	return EBossState::NONE;
}

void UDeflectedState::Stop()
{
	Super::Stop();
}

EBossState UDeflectedState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (!CanStartMovement()) return EBossState::NONE;

	if (TotalRunTime <= MaxRunTime)
	{
		TotalRunTime += DeltaTime;

		FVector Loc = FMath::Lerp(PrevLoc, NewLoc, EaseOutExpo(TotalRunTime / MaxRunTime));

		Instigator->SetActorLocation(Loc);
	}

	return EBossState::NONE;
}
