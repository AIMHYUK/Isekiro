// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/FSMComponent.h"
#include "FSM/StateObject.h"
#include "FSM/BossStates/StrafeState.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StateToStart = EBossState::STRAFE;
}


void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	if (ensure(StateToStart != EBossState::NONE))
	{
		PrepNewState(StateToStart);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Starting State has not been set. Boss will not perform any actions."));
	}
}

void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentState)
	{
		EBossState NewState = CurrentState->Update(DeltaTime);
		if (NewState != EBossState::NONE)
		{
			PrepNewState(NewState);
		}
	}
}

void UFSMComponent::ChangeStateTo(EBossState NewState)
{
	PrepNewState(NewState);
}

void UFSMComponent::PrepNewState(EBossState NewState)
{
	TSubclassOf<UStateObject>* StateObjectClass = BossStateMap.Find(NewState);
	if (StateObjectClass)
	{
		UStateObject* NewStateObj = NewObject<UStateObject>(GetOwner(), *StateObjectClass);
		if (NewStateObj)
		{
			NewStateObj->Initialize(GetOwner(), Target);
			if (CurrentState) CurrentState->Stop();
			NewStateObj->Start();
			CurrentState = NewStateObj;
		}
	}
}