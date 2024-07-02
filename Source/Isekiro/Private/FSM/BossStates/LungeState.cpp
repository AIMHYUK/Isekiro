// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/LungeState.h"
#include "Character/BossCharacter.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimCompositeBase.h"

ULungeState::ULungeState()
{
	bHasPrevLoc = false;
	PrevLoc = FVector::Zero();

	MaxRunTime = .9f;
	TotalRunTime = 0.f;
}

void ULungeState::Start()
{
	Super::Start();
}

EBossState ULungeState::Update(float DeltaTime)
{
	EBossState State = Super::Update(DeltaTime);

	return State;
}

void ULungeState::Stop()
{
}

EBossState ULungeState::UpdateMovement(float DeltaTime)
{
	if (!CanStartMovement())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Test: %s"), CanStartMovement() ? TEXT("YES") : TEXT("NO")));
		return EBossState::NONE;
	}

	if (!bHasPrevLoc)
	{
		bHasPrevLoc = true;
		PrevLoc = Instigator->GetActorLocation();
	}

	FVector lungeVector = Instigator->GetActorLocation();
	FVector ToTarget = Instigator->GetTargetOffsetLocation();
	DrawDebugSphere(GetWorld(), ToTarget, 5.f, 12, FColor::Blue, true);
	if (TotalRunTime < MaxRunTime) {
	
		TotalRunTime += DeltaTime / 2.f;

		lungeVector = FMath::Lerp(PrevLoc, ToTarget, EaseOutSine(TotalRunTime / MaxRunTime));
	}

	Instigator->SetActorLocation(lungeVector);

	return EBossState::NONE;
}
