// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/AttackState.h"
#include "Character/BossCharacter.h"

UAttackState::UAttackState()
{
	MoveSpeed = 70.f;
	StateDistance.Max = 350.f;
}

void UAttackState::Start()
{
	Super::Start();

	if (Instigator)
	{
		UAnimInstance* AnimInstance = Instigator->GetMesh()->GetAnimInstance();

		//If player is within range, continue with in-place animation. Otherwise, jump to root animation
		if (AnimInstance) 
		{
			FName Section = AnimInstance->Montage_GetCurrentSection();
			if (Section.ToString().IsEmpty()) return;

			FString SectionS = Section.ToString();
			int32 num = FCString::Atoi(*SectionS);

			/*Is target within attack range*/
			if (Instigator->IsWithinTarget()) // play in-place anim sequence
			{
				//Start Movement in FSM
				StartMovement();
			}
			else // play root anim sequence
			{
				SectionS.Append("_r");
				//Stop Movement in FSM
				StopMovement();
			}

			AnimInstance->Montage_JumpToSection(FName(SectionS), AnimInstance->GetCurrentActiveMontage());
		}
	}	
}

EBossState UAttackState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if(FSMComp && !FSMComp->IsCurrentStateActive()) return FSMComp->RandomState();
	return EBossState::NONE;
}

void UAttackState::Stop()
{
	Super::Stop();
	StopMovement();
}

EBossState UAttackState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (!CanStartMovement()) return EBossState::NONE;

	if (!Instigator->IsWithinTarget())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Testing")));
		FVector DirVector = Instigator->GetTargetOffsetLocation() - Instigator->GetTargetLocation();

		DirVector.Normalize();

		Instigator->SetActorLocation(DirVector * MoveSpeed * DeltaTime + Instigator->GetActorLocation());
	}
	
	return EBossState::NONE;
}
