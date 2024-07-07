// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/HitState.h"
#include "Character/BossCharacter.h"

UHitState::UHitState()
{
	MaxRunTime = .6f;
	TotalRunTime = 0.f;
	TravelDistance = 50.f;
}

void UHitState::Start()
{
	Super::Start();

	PrevLoc = Instigator->GetActorLocation();

	FVector TargetLoc = Instigator->GetTargetOffsetLocation();
	FVector DirVector = PrevLoc - TargetLoc;
	DirVector.Normalize();
	NewLoc = DirVector * TravelDistance + PrevLoc;
	NewLoc.Z += FMath::RandRange(-5.f, 5.f);

	int32 SectionNum = FMath::RandRange(1, 4);
	Instigator->GetMesh()->GetAnimInstance()->Montage_JumpToSection(
		FName(FString::FromInt(SectionNum)), MontageState.Montage);
}

EBossState UHitState::Update(float DeltaTime)
{
	return Super::Update(DeltaTime);
}

void UHitState::Stop()
{
	Super::Stop();
}

EBossState UHitState::UpdateMovement(float DeltaTime)
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
