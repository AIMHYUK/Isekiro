// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/StateObject.h"
#include "Character/BossCharacter.h"

UStateObject::UStateObject()
{
	bIsMoving = false;
}

void UStateObject::Initialize(ABossCharacter* _Instigator, AActor* _Target)
{
	Instigator = _Instigator;
	Target = _Target;
}

void UStateObject::Start()
{
	PlayMontage();
}

EBossState UStateObject::Update(float DeltaTime)
{
	return UpdateMovement(DeltaTime);
}

void UStateObject::Stop()
{

}

void UStateObject::Activate()
{
}

void UStateObject::PlayMontage()
{
	if (Instigator && MontageState.Montage)
	{
		//Instigator->GetMesh()->GetAnimInstance()->Montage_Play(MontageState.Montage);
		Instigator->PlayAnimMontage(MontageState.Montage, 1, MontageState.SectionName);
		//if (!MontageState.SectionName.IsNone())
		//	Instigator->GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageState.SectionName, MontageState.Montage);
	}
}

bool UStateObject::CanStartMovement() const
{
	return bIsMoving;
}

EBossState UStateObject::UpdateMovement(float DeltaTime)
{
	return EBossState::NONE;
}

void UStateObject::StartMovement()
{
	bIsMoving = true;
}

void UStateObject::StopMovement()
{
	bIsMoving = false;
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
