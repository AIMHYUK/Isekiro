// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/FSMComponent.h"
#include "FSM/StateObject.h"
#include "FSM/BossStates/StrafeState.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentStateE = EBossState::STRAFE;
	RandomStateCount = -1;
	
	bCanStun = true;

	StunMaxTax = 10.f;
	StunTaxRate = 2.f;
	StunTaxTotal = 0.f;
}


void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* Boss = Cast<ABossCharacter>(GetOwner());
	if (Boss)
	{
		Target = Boss->GetTarget();
		BossCharacter = Boss;
	}

	if (ensure(CurrentStateE != EBossState::NONE))
	{
		ChangeStateTo(CurrentStateE);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Starting State has not been set. Boss will not perform any actions."));
	}
}

void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(CurrentState))
	{
		EBossState NewState = CurrentState->Update(DeltaTime);
		if (NewState != EBossState::NONE)
		{
			ChangeStateTo(NewState);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("CurrentState: %s"), *GetNameSafe(CurrentState)));
}

EBossState UFSMComponent::RandomState()
{
	int32 size = (int32)EBossState::MAX;
	int index;
	do
	{
		index = FMath::RandRange(0, size - 1);
		RandomStateCount++;
	} while (EBossState(index) == EBossState::HIT || EBossState(index) == EBossState::PARRY || !CanChangeStateTo((EBossState)index) && RandomStateCount < 50);

	if (RandomStateCount >= 50)
	{
		RandomStateCount = -1;
		return EBossState::DODGE;
	}

	RandomStateCount = -1;
	return EBossState(index);
}

bool UFSMComponent::IsCurrentStateActive() const
{
	return BossCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
}

void UFSMComponent::SetFSMState(EBossState _CurrentStateE)
{
	CurrentStateE = _CurrentStateE;
}

bool UFSMComponent::CanChangeStateTo(EBossState StateToTest)
{
	if (StateToTest == EBossState::NONE) return false;

	if (StateToTest == CurrentStateE) return false;

	TSubclassOf<UStateObject>* StateObjectClass = BossStateMap.Find(StateToTest);
	if (StateObjectClass && *StateObjectClass)
	{
		TSubclassOf<UStateObject> StateObjClass = *StateObjectClass;
		FStateDistance StateDist = StateObjClass->GetDefaultObject<UStateObject>()->GetStateDistance();

		if (BossCharacter)
		{
			float DistToTarget = BossCharacter->GetDistanceToTargetOffset();
			return DistToTarget >= StateDist.Min && DistToTarget <= StateDist.Max;
		}
	}
	return false;
}

void UFSMComponent::ChangeStateTo(EBossState NewState)
{
	PrepNewState(NewState) ? CurrentStateE = NewState : CurrentStateE = EBossState::NONE;
}

void UFSMComponent::StartMovement()
{
	if (CurrentState)
	{
		CurrentState->StartMovement();
	}
}

void UFSMComponent::StopMovement()
{
	if (CurrentState)
	{
		CurrentState->StopMovement();
	}
}

EBossState UFSMComponent::GetCurrentStateE() const
{
	return CurrentStateE;
}

bool UFSMComponent::CanStun() const
{
	return bCanStun;
}

void UFSMComponent::EnableStun(bool bStun)
{
	bCanStun = bStun;
}

bool UFSMComponent::CanParry() const
{
	int value = 0;
	switch (CurrentStateE)
	{
	case EBossState::NONE:
		return false;
	case EBossState::STRAFE:
		return true;
	case EBossState::RUN:
		value = FMath::RandRange(1, 10);
		return value <= 8; // 80% chance to block while running.

	/*add another case for Boss normal attack state
	if (BossCharacter)
	{
		auto* StatusComp = BossCharacter->GetComponentByClass<UStatusComponent>();
		if (StatusComp)
		{
			value = FMath::RandRange(1, 10);
			if (StatusComp->GetPosturePercent() >= .7f)
				return value <= 5;
			else return value <= 7;
		}
	}*/

	default:
	{
		if (BossCharacter)
		{
			auto* StatusComp = BossCharacter->GetComponentByClass<UStatusComponent>();
			if (StatusComp)
			{
				value = FMath::RandRange(1.f, 10.f);
				if (StatusComp->GetPosturePercent() >= .7f) 
					return value <= 1.0f;
				else return value <= 2.5f;
			}
		}
	}
	}

	return false;
}

bool UFSMComponent::PrepNewState(EBossState NewState)
{
	TSubclassOf<UStateObject>* StateObjectClass = BossStateMap.Find(NewState);
	if (StateObjectClass && *StateObjectClass)
	{
		UStateObject* NewStateObj = NewObject<UStateObject>(GetOwner(), *StateObjectClass);
		if (NewStateObj)
		{
			NewStateObj->Initialize(this, Cast<ABossCharacter>(GetOwner()), Target);
			if (CurrentState) CurrentState->Stop();
			NewStateObj->Start();
			CurrentState = NewStateObj;
			EBossState NewStateE = NewStateObj->GetFSMState();
			if (ensure(NewStateE != EBossState::NONE && NewStateE != EBossState::MAX))
			{
				SetFSMState(NewStateObj->GetFSMState());
				return true;
			}
		}
		else UE_LOG(LogTemp, Warning, TEXT("Could not Instantiate new boss State object."));
	}
	else UE_LOG(LogTemp, Warning, TEXT("Could not find Boss State in BossStateMap."));

	return false;
}