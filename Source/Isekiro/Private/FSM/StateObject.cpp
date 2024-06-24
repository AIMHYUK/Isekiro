// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/StateObject.h"
#include "Character/BossCharacter.h"

void UStateObject::Initialize(AActor* _Instigator, AActor* _Target)
{
	Instigator = _Instigator;
	Target = _Target;
}

void UStateObject::Start()
{

}

UStateObject* UStateObject::Update(float DeltaTime)
{
	return nullptr;
}

void UStateObject::Stop()
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
