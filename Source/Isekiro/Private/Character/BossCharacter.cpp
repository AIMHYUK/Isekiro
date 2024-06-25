// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"
#include "FSM/FSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FSM/StateObject.h"

ABossCharacter::ABossCharacter()
{
	FSMComponent = CreateDefaultSubobject<UFSMComponent>("FSMComponent");

	TargetOffset = 150.f;
	StrafeSpeed = 100.f;
	StrafeMaxTime = 3.f;
	StrafeTotalTime = 0.f;

	RunSpeed = 400.f;
	bHasPrevLoc = false;
	PrevLoc = FVector::Zero();

	MaxRunTime = 2.f;
	TotalRunTime = 0.f;
	JumpHeight = 120.f;
	JumpMaxTime = .5f;
	JumpTotalTime = 0.f;

	CurrentState = ETestState::Strafe;
	StateMaxTime = 5.f;
}


void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensureAlways(TargetO))
	{
		FVector DirVector = TargetO->GetActorLocation() - GetActorLocation();
		DirVector.Normalize();
		FRotator newRotation = DirVector.Rotation();
		newRotation.Pitch = 0.f;
		newRotation.Roll = 0.f;
		SetActorRotation(newRotation.Quaternion());
	}

	//TotalTime += DeltaTime;
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Time: %.2f"), TotalTime));
	//if (TotalTime >= StateMaxTime)
	//{
	//	TotalTime = 0.f;
	//	ETestState test;
	//	do {
	//		test = ETestState(FMath::RandRange(0, 3));
	//	} while (test == CurrentState);
	//	CurrentState = test;
	//}

	//switch (CurrentState)
	//{
	//case ETestState::Strafe:
	//{
	//	StrafeTotalTime += DeltaTime;
	//	if (StrafeTotalTime < StrafeMaxTime)
	//	{
	//		FVector newLoc = GetActorLocation() + GetActorRightVector() * StrafeSpeed * DeltaTime;
	//		SetActorLocation(newLoc);
	//	}
	//	else
	//	{
	//		StrafeTotalTime = 0.f;
	//		CurrentState = ETestState::Lunge;
	//	}
	//	break;
	//}
	//case ETestState::Run:
	//{
	//	float Dist = GetDistanceToTarget();
	//	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Distance: %.2f"), Dist));

	//	if (Dist > TargetOffset)
	//	{
	//		FVector newLoc = GetActorLocation() + GetActorForwardVector() * RunSpeed * DeltaTime;
	//		SetActorLocation(newLoc);
	//	}

	//	break;
	//}
	//case ETestState::Rush:
	//{
	//	CurrentState = ETestState::Run;
	//	break;
	//}
	//case ETestState::Lunge:
	//{
	//	if (!bHasPrevLoc)
	//	{
	//		bHasPrevLoc = true;
	//		PrevLoc = GetActorLocation();
	//	}
	//	FVector lungeVector;
	//	FVector jumpVector;
	//	if (TotalRunTime < MaxRunTime) {
	//		TotalRunTime += DeltaTime;

	//		FVector ToTarget = GetTargetOffsetLocation();
	//		ToTarget.Z = GetActorLocation().Z;
	//		lungeVector = FMath::Lerp(PrevLoc, ToTarget, EaseOutSine(TotalRunTime / MaxRunTime));
	//		
	//		//if (JumpTotalTime < JumpMaxTime) {
	//		//	JumpTotalTime += DeltaTime;

	//		//	FVector jumpDest = GetActorUpVector() * JumpHeight;
	//		//	jumpVector = FMath::Lerp(PrevLoc, jumpDest, EaseOutSine(JumpTotalTime / JumpMaxTime));
	//		//}

	//		SetActorLocation(lungeVector + jumpVector);
	//	}

	//	break;
	//}
	//}
}

float ABossCharacter::GetDistanceToTarget() const
{
	if (TargetO)
	{
		FVector DirVector = TargetO->GetActorLocation() - GetActorLocation();
		return FMath::Sqrt(DirVector.Dot(DirVector));
	}

	return -1.f;
}

FVector ABossCharacter::GetTargetOffsetLocation() const
{
	if (TargetO)
	{
		FVector WorkingVector = TargetO->GetActorForwardVector();
		WorkingVector *= TargetOffset;
		return WorkingVector;
	}
	return FVector();
}
