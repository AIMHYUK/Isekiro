// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/ParryState.h"
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

	if (Instigator)
	{
		Instigator->SetLockOnTarget(true);
	}

	PrevLoc = Instigator->GetActorLocation();
	FVector TargetLoc = Instigator->GetTargetLoc();

	float Distance = FVector::Distance(PrevLoc, TargetLoc);
	FVector DirVec = PrevLoc - TargetLoc;
	DirVec.Normalize();

	if(Distance < Instigator->GetTargetOffset())
		NewLoc = Instigator->GetTargetOffset() * DirVec + TargetLoc;
	else 
		NewLoc = Distance * DirVec + TargetLoc;

	NewLoc = 200.f * DirVec + TargetLoc;

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
