// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"
#include "FSM/FSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FSM/StateObject.h"
#include "Components/BoxComponent.h"

ABossCharacter::ABossCharacter()
{
	FSMComponent = CreateDefaultSubobject<UFSMComponent>("FSMComponent");

	AttackBoxComp = CreateDefaultSubobject<UBoxComponent>("AttackBoxComponent");
	AttackBoxComp->SetupAttachment(RootComponent);
	AttackBoxComp->SetRelativeLocation(FVector(100.f, 0.f, 0.f));
	AttackBoxComp->SetBoxExtent(FVector(75.f, 50.f, 75.f));

	TargetOffset = 150.f;
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AttackBoxComp)
	{
		AttackBoxComp->IgnoreActorWhenMoving(this, true);

		AttackBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnAttackBoxOverlapped);
		AttackBoxComp->OnComponentDeactivated.AddDynamic(this, &ABossCharacter::OnDeactivated);
		AttackBoxComp->SetActive(true);
	}
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttackBoxComp->IsActive()) 
	{
		AttackBoxComp->Deactivate();
	}

	if (ensureAlways(TargetO))
	{
		FVector DirVector = TargetO->GetActorLocation() - GetActorLocation();
		DirVector.Normalize();
		FRotator newRotation = DirVector.Rotation();
		newRotation.Pitch = 0.f;
		newRotation.Roll = 0.f;
		SetActorRotation(newRotation.Quaternion());
	}
}

void ABossCharacter::BeginAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Attack"));
	AttackBoxComp->Activate();
}

void ABossCharacter::OnAttackBoxOverlapped(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping with %s"), *OtherActor->GetName());
	AttackBoxComp->Deactivate();
}

void ABossCharacter::OnDeactivated(UActorComponent* Component)
{
	UE_LOG(LogTemp, Warning, TEXT("Deactivated"));
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
