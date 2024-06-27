// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/StateObject.h"
#include "Character/BossCharacter.h"

void UStateObject::Initialize(ABossCharacter* _Instigator, AActor* _Target)
{
	Instigator = _Instigator;
	Target = _Target;
}

void UStateObject::Start()
{

}

EBossState UStateObject::Update(float DeltaTime)
{
	return EBossState::NONE;
}

void UStateObject::Stop()
{

}

void UStateObject::Activate()
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
