// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/LungeState.h"
#include "Character/BossCharacter.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimCompositeBase.h"

ULungeState::ULungeState()
{
	bHasPrevLoc = false;
	PrevLoc = FVector::Zero();

	StateDistance.Min = 400.f;
	StateDistance.Max = 800.f;

	MaxRunTime = 1.35f;
	TotalRunTime = 0.f;
}

void ULungeState::Start()
{
	Super::Start();
}

EBossState ULungeState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
}

EBossState ULungeState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (!bHasPrevLoc)
	{
		bHasPrevLoc = true;
		PrevLoc = Instigator->GetActorLocation();
	}

	FVector lungeVector = Instigator->GetActorLocation();
	FVector ToTarget = Instigator->GetTargetOffsetLocation();
	if (TotalRunTime < MaxRunTime) {
	
		TotalRunTime += DeltaTime;

		lungeVector = FMath::Lerp(PrevLoc, ToTarget, TotalRunTime / MaxRunTime);
	}

	Instigator->SetActorLocation(lungeVector);

	return EBossState::NONE;
}
