// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/ParryState.h"
#include "Character/BossCharacter.h"

UParryState::UParryState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	TravelDist = 80.f;
	StateDistance.Max = 350.f;
}

void UParryState::Start()
{
	Super::Start();

	PrevLoc = Instigator->GetActorLocation();

	FVector TargetLoc = Instigator->GetTargetOffsetLocation();
	FVector DirVector = PrevLoc - TargetLoc;
	DirVector.Normalize();
	NewLoc = DirVector * TravelDist + PrevLoc;
	NewLoc.Z += FMath::RandRange(-5.f, 5.f);

	int32 SectionNum = FMath::RandRange(1,4);
	Instigator->GetMesh()->GetAnimInstance()->Montage_JumpToSection(
		FName(FString::FromInt(SectionNum)), MontageState.Montage);
}

EBossState UParryState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	
	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
}

void UParryState::Stop()
{
	Super::Stop();
}

EBossState UParryState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if(!CanStartMovement()) return EBossState::NONE;

	if (TotalRunTime <= MaxRunTime)
	{
		TotalRunTime += DeltaTime;

		FVector Loc = FMath::Lerp(PrevLoc, NewLoc, EaseOutExpo(TotalRunTime / MaxRunTime));

		Instigator->SetActorLocation(Loc);
	}

	return EBossState::NONE;
}
