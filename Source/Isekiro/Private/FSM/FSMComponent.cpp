// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/FSMComponent.h"
#include "FSM/StateObject.h"
#include "FSM/BossStates/StrafeState.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	if (StateToStart)
	{
		CurrentState = NewObject<UStrafeState>(GetOwner(), StateToStart);
		if(CurrentState) 
			CurrentState->Initialize(GetOwner(), Target);
	}
}


void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentState)
	{
		UStateObject* NewState = CurrentState->Update(DeltaTime);
		if (NewState)
		{
			NewState->Initialize(GetOwner(), Target);
			CurrentState->Stop();
			NewState->Start();
			CurrentState = NewState;
		}
	}
}

