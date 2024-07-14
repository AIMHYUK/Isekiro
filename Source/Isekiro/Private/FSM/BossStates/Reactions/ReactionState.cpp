// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/Reactions/ReactionState.h"
#include "Character/BossCharacter.h"

UReactionState::UReactionState()
{
	MaxRunTime = .5f;
	TotalRunTime = 0.f;
	StateDistance.Max = 350.f;

	TravelDist = 250.f;

	PrevState = EBossState::NONE;

	ResponseProbability = 90.f;

	//MeleeAttackProb = 1.f;
	//NormalAttackProb = 1.f;
}

void UReactionState::Start()
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

	NewLoc = TravelDist * DirVec + TargetLoc;
}

EBossState UReactionState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (FSMComp && !FSMComp->IsCurrentStateActive())
	{
		return FSMComp->RandomState();
	}
	return EBossState::NONE;
}

void UReactionState::Stop()
{
	Super::Stop();
	StopMovement();
	if (Instigator && Instigator->GetMesh() && Instigator->GetMesh()->GetAnimInstance())
	{
		auto Anim = Instigator->GetMesh()->GetAnimInstance();
		if (Anim && Anim->IsAnyMontagePlaying())
		{
			Anim->StopAllMontages(0.1f);
		}
	}
}

EBossState UReactionState::UpdateMovement(float DeltaTime)
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

void UReactionState::RespondToInput()
{
	Super::RespondToInput();

	StopMovement();

	if(FMath::RandRange(0.f, 100.f) > ResponseProbability) return;

	if (!FSMComp) return;

	SetupCulumativeProbability();

	EBossState NextState = GetNextState();

	FSMComp->ChangeStateTo(NextState);
}

void UReactionState::SetupCulumativeProbability()
{
	float Total = 0.f;

	for (int i = 0; i < Probabilities.Num(); ++i)
	{
		Total += Probabilities[i].Probability;
		Cumulative.Add(FStateProbability{ Probabilities[i].BossState,Total });
	}
	if (!ensureAlways(Total == 100.f))
		UE_LOG(LogTemp, Warning, TEXT("Near Probabilities does not add up to 100%%"));
}

EBossState UReactionState::GetNextState()
{
	int32 Index = -1;
	do
	{
		float Selected = FMath::RandRange(0.f, 100.f);

		for (int i = 0; i < Cumulative.Num(); ++i)
		{
			if (Selected <= Cumulative[i].Probability)
			{
				Index = i;
				break;
			}
		}
		if (!Cumulative.IsValidIndex(Index)) return EBossState::NONE;

	} while (Cumulative[Index].BossState == PrevState);


	if (Cumulative[Index].BossState != EBossState::NORMALATTACK)
		PrevState = Cumulative[Index].BossState;

	return Cumulative[Index].BossState;
}
