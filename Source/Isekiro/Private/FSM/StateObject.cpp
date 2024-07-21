// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/StateObject.h"
#include "Character/BossCharacter.h"
#include "Animation/AnimInstance.h"

UStateObject::UStateObject()
{
	bIsMoving = false;
	StateDistance.Min = 0.f;
	StateDistance.Max = 50000.f;
	FSMState = EBossState::NONE;
	SelectedIndex = -1;
}

void UStateObject::Initialize(UFSMComponent* _FSMComp, ABossCharacter* _Instigator, AActor* _Target)
{
	FSMComp = _FSMComp;
	Instigator = _Instigator;
	Target = _Target;
}

void UStateObject::Start()
{
	EBossState NewStateE = GetFSMState();
	if (FSMComp && (NewStateE != EBossState::NONE && NewStateE != EBossState::MAX))
	{
		FSMComp->SetCurrentState(GetFSMState());
	}

	PlayMontage();
	UE_LOG(LogTemp, Warning, TEXT("42 Start: %s"), *UEnum::GetValueAsString(GetFSMState()));
}

EBossState UStateObject::Update(float DeltaTime)
{
	if (CanStartMovement()) return UpdateMovement(DeltaTime);
	return EBossState::NONE;
}

void UStateObject::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("42 Stop: %s"), *UEnum::GetValueAsString(GetFSMState()));
	StopMontage();
	StopMovement();

	if (Instigator) Instigator->EquipKatana();
}

void UStateObject::Activate()
{
}

UAnimMontage* UStateObject::GetMontage() const
{
	if (Instigator && SelectedIndex != -1)
	{
		return MontageStates[SelectedIndex].Montage;
	}

	return nullptr;
}

UAnimInstance* UStateObject::GetAnimInstance() const
{
	if (Instigator)
	{
		auto Anim = Instigator->GetMesh()->GetAnimInstance();
		{
			if (Anim) return Anim;
		}
	}
	return nullptr;
}

void UStateObject::JumpToSection(FName SectionName)
{
	if (GetAnimInstance() && GetMontage())
	{
		GetAnimInstance()->Montage_JumpToSection(SectionName, GetMontage());
	}
}

void UStateObject::PlayMontage()
{
	if (MontageStates.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Anim Montage to play in %s state"), *GetNameSafe(this));
		return;
	}

	SelectedIndex = FMath::RandRange(0, MontageStates.Num() - 1);

	if (Instigator)
	{
		if (MontageStates[SelectedIndex].Montage)
		{
			Instigator->GetMesh()->GetAnimInstance()->Montage_Play(MontageStates[SelectedIndex].Montage);
			if (!MontageStates[SelectedIndex].SectionName.IsNone())
				Instigator->GetMesh()->GetAnimInstance()->Montage_JumpToSection(
					MontageStates[SelectedIndex].SectionName, MontageStates[SelectedIndex].Montage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Anim Montage is null in %s state"), *GetNameSafe(this));
			SelectedIndex = -1;
		}
	}
}

void UStateObject::StopMontage()
{
	if (GetMontage())
	{
		auto Anim = Instigator->GetMesh()->GetAnimInstance();
		{
			if (Anim)
			{
				Anim->Montage_Stop(0.1f, GetMontage());
			}
		}
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