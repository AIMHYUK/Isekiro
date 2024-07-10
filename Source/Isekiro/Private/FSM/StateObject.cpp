// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/StateObject.h"
#include "Character/BossCharacter.h"

UStateObject::UStateObject()
{
	bIsMoving = false;
	StateDistance.Min = 0.f;
	StateDistance.Max = 50000.f;
	FSMState = EBossState::NONE;
}

void UStateObject::Initialize(UFSMComponent* _FSMComp, ABossCharacter* _Instigator, AActor* _Target)
{
	FSMComp = _FSMComp;
	Instigator = _Instigator;
	Target = _Target;
}

void UStateObject::Start()
{
	PlayMontage();
}

EBossState UStateObject::Update(float DeltaTime)
{
	return UpdateMovement(DeltaTime);
}

void UStateObject::Stop()
{
}

void UStateObject::Activate()
{
}

void UStateObject::PlayMontage()
{
	if (Instigator && MontageState.Montage)
	{
		Instigator->GetMesh()->GetAnimInstance()->Montage_Play(MontageState.Montage);
		if (!MontageState.SectionName.IsNone())
			Instigator->GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageState.SectionName, MontageState.Montage);
	}
}

bool UStateObject::CanStartMovement() const
{
	return bIsMoving;
}

EBossState UStateObject::UpdateMovement(float DeltaTime)
{
	return EBossState::NONE;
}

void UStateObject::StartMovement()
{
	bIsMoving = true;
}

void UStateObject::StopMovement()
{
	bIsMoving = false;
}

EBossState UStateObject::GetFSMState() const
{
	return FSMState;
}

FStateDistance UStateObject::GetStateDistance() const
{
	return StateDistance;
}

void UStateObject::RespondToInput()
{
}


UWorld* UStateObject::GetWorld() const
{
	Super::GetWorld();
	AActor* Actor = Cast<AActor>(GetOuter());

	if (Actor)
	{
		return Actor->GetWorld();
	}

	UE_LOG(LogTemp, Warning, TEXT("No valid outer found in %s. Returning null UWorld*"), *GetNameSafe(this));
	return nullptr;
}