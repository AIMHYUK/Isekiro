// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/FSMComponent.h"
#include "FSM/StateObject.h"
#include "FSM/BossStates/StrafeState.h"
#include "Character/BossCharacter.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentStateE = EBossState::STRAFE;
}


void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* Boss = Cast<ABossCharacter>(GetOwner());
	if (Boss)
	{
		Target = Boss->GetTarget();
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

bool UFSMComponent::PrepNewState(EBossState NewState)
{
	TSubclassOf<UStateObject>* StateObjectClass = BossStateMap.Find(NewState);
	if (StateObjectClass && *StateObjectClass)
	{
		UStateObject* NewStateObj = NewObject<UStateObject>(GetOwner(), *StateObjectClass);
		if (NewStateObj)
		{
			NewStateObj->Initialize(Cast<ABossCharacter>(GetOwner()), Target);
			if (CurrentState) CurrentState->Stop();
			NewStateObj->Start();
			CurrentState = NewStateObj;
			return true;
		}
		else UE_LOG(LogTemp, Warning, TEXT("Could not Instantiate new boss State object."));
	}
	else UE_LOG(LogTemp, Warning, TEXT("Could not find Boss State in BossStateMap."));

	return false;
}