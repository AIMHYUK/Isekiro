// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/StrafeState.h"
#include "GameFramework/Actor.h"

UStrafeState::UStrafeState()
{
	StrafeSpeed = 100.f;
}

void UStrafeState::Start()
{
	Super::Start();
}

UStateObject* UStrafeState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (Instigator)
	{
		FVector newLoc = Instigator->GetActorLocation() + Instigator->GetActorRightVector() * StrafeSpeed * DeltaTime;

		Instigator->SetActorLocation(newLoc);
	}
	return nullptr;
}

void UStrafeState::Stop()
{
	Super::Stop();
}
