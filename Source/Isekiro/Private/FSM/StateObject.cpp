// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/StateObject.h"
#include "Character/BossCharacter.h"

void UStateObject::Initialize()
{
	
}

void UStateObject::TestFunction()
{
	if (GetWorld())
	{
		if (BossCharacterClass) {
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(BossCharacterClass, FTransform::Identity, Params);
		}
	}
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
