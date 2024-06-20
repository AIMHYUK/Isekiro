// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"
#include "ActorComponents/FSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABossCharacter::ABossCharacter()
{
	FSMComponent = CreateDefaultSubobject<UFSMComponent>("FSMComponent");

	BufferDist = 150.f;
	StrafeSpeed = 100.f;
	RunSpeed = 500.f;

	CurrentState = ETestState::Strafe;
	StateMaxTime = 5.f;
}


void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensureAlways(Target)) 
	{
		FVector DirVector = Target->GetActorLocation() - GetActorLocation();
		DirVector.Normalize();
		FRotator newRotation = DirVector.Rotation();
		newRotation.Pitch = 0.f; 
		newRotation.Roll = 0.f; 
		SetActorRotation(newRotation.Quaternion());
	}

	{
		TotalTime += DeltaTime;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Time: %.2f"), TotalTime));
		if (TotalTime >= StateMaxTime) 
		{
			TotalTime = 0.f;
			ETestState test;
			do {
				test = ETestState(FMath::RandRange(0, 3)); 
			} while (test == CurrentState);			
			CurrentState = test;
		}
	}

	switch (CurrentState) 
	{
	case ETestState::Strafe: 
	{
		FVector newLoc = GetActorLocation() + GetActorRightVector() * StrafeSpeed * DeltaTime;

		SetActorLocation(newLoc);
		break;
	}
	case ETestState::Run:
	{
		float Dist = GetDistanceToTarget();
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Distance: %.2f"), Dist));
		
		if (Dist > BufferDist) 
		{
			FVector newLoc = GetActorLocation() + GetActorForwardVector() * RunSpeed * DeltaTime;
			SetActorLocation(newLoc);

			//FMath::Lerp();
		}

		break;
	}
	case ETestState::Rush:
	{
		break;
	}
	case ETestState::Lunge:
	{
		break;
	}
	}
}

float ABossCharacter::GetDistanceToTarget() const
{
	if (Target) 
	{
		FVector DirVector = Target->GetActorLocation() - GetActorLocation();
		return FMath::Sqrt(DirVector.Dot(DirVector));
	}
	
	return -1.f;
}
