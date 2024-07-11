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

	bCanStun = true;

	DodgeMaxProb = 100.f;
	DodgeProbRate = 3.f;
	DodgeProbTotal = 0.f;

	FightSpace = EFightingSpace::FAR;

	ParryProbability = .8f;
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

	if (CanStun())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Yes Can Stun")));
	}
	else GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("No Can't Stun")));

	switch(FightSpace)
	{
	case EFightingSpace::FAR:
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("FAR")));
		break;
	case EFightingSpace::NEAR:
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("NEAR")));
		break;
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("CurrentState: %s"), *GetNameSafe(CurrentState)));
}

EBossState UFSMComponent::RandomState()
{
	int index;
	int32 min = 0;
	int32 max = 0;

	if (BossCharacter && !BossCharacter->IsWithinNearRange()) FightSpace = EFightingSpace::FAR;

	switch (FightSpace)
	{
	case EFightingSpace::NEAR:
		min = (int32)EBossState::NORMALATTACK;
		max = (int32)EBossState::STRAFE;
		break;
	case EFightingSpace::FAR:
		min = (int32)EBossState::STRAFE;
		max = (int32)EBossState::LUNGEATTACK;
		break;
	}

	int32 RandomStateCount = 0;
	do
	{
		index = FMath::RandRange(min, max);
		RandomStateCount++;
	} while (EBossState(index) == EBossState::HIT || EBossState(index) == EBossState::PARRY || !CanChangeStateTo((EBossState)index) && RandomStateCount < 50);

	if (RandomStateCount >= 50)
	{
		return EBossState::DODGE;
	}

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

void UFSMComponent::RespondToState()
{
	if (CurrentState)
	{
		CurrentState->RespondToInput();
	}
}

void UFSMComponent::RespondToInput()
{
	if (CurrentState)
	{
		CurrentState->RespondToInput();
	}
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

bool UFSMComponent::IsPostureBroken() const
{
	return PostureState == EPostureState::BROKEN;
}

void UFSMComponent::SetPostureState(EPostureState _PostureState)
{
	PostureState = _PostureState;
}

bool UFSMComponent::CanTakeDamage()
{
	if(HandleDodgeProbability())
	{
		return true;
	}
	else
	{
		RespondToDamageTakenFailed();
		return false;
	}	
}

bool UFSMComponent::HandleDodgeProbability()
{
	if (FMath::RandRange(0.f, 1.f) >= DodgeProbTotal / DodgeMaxProb) // Take Damage
	{
		FightSpace = EFightingSpace::NEAR;
		DodgeProbTotal += DodgeProbRate;
		return true;
	}
	else
	{
		FightSpace = EFightingSpace::FAR;
		DodgeProbTotal = 0.f;
		return false;
	}
}

void UFSMComponent::RespondToDamageTakenFailed()
{
	switch (FMath::RandRange(0, 1))
	{
	case 0:
	{
		ChangeStateTo(EBossState::DODGEATTACK);
		break;
	}
	case 1:
		ChangeStateTo(EBossState::DODGE);
		break;
	}
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
	float value = 0.f;
	switch (CurrentStateE)
	{
	case EBossState::NONE:
		return false;
	case EBossState::STRAFE:
		return true;
	case EBossState::RUN:
		value = FMath::RandRange(0.f, 1.f);
		return value <= .8f; // 80% chance to block while running.

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
	case EBossState::DEFLECTED:
		value = FMath::RandRange(0.f, 1.f);
		return value <= .1f; // 10% chance to block when Deflected.
	default:
	{
		if (BossCharacter)
		{
			auto* StatusComp = BossCharacter->GetComponentByClass<UStatusComponent>();
			if (StatusComp)
			{
				if (StatusComp->GetPosturePercent() >= .7f) return value <= ParryProbability;
				else return value <= ParryProbability;
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