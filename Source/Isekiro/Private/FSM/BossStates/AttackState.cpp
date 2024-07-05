// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/AttackState.h"
#include "Character/BossCharacter.h"

UAttackState::UAttackState()
{
	MoveSpeed = 70.f;
}

void UAttackState::Start()
{
	Super::Start();

	if (Instigator)
	{
		UAnimInstance* AnimInstance = Instigator->GetMesh()->GetAnimInstance();
		if (AnimInstance) 
		{
			FName Section = AnimInstance->Montage_GetCurrentSection();
			if (Section.ToString().IsEmpty()) return;

			FString SectionS = Section.ToString();
			int32 num = FCString::Atoi(*SectionS);
			num++;
			SectionS = FString::FromInt(num);

			/*Is target within attack range*/
			if (Instigator->IsWithinAttackRange()) // play in-place anim sequence
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
	return Super::Update(DeltaTime);
}

void UAttackState::Stop()
{
	Super::Stop();
}

EBossState UAttackState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (!CanStartMovement()) return EBossState::NONE;

	if (!Instigator->IsWithinTargetOffsetBuffer())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Testing")));
		FVector DirVector = Instigator->GetTargetOffsetLocation() - Instigator->GetTargetLocation();

		DirVector.Normalize();

		Instigator->SetActorLocation(DirVector * MoveSpeed * DeltaTime + Instigator->GetActorLocation());
	}
	
	return EBossState::NONE;
}
