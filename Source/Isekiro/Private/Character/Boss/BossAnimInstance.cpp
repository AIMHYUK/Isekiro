// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/BossAnimInstance.h"
#include "FSM/FSMComponent.h"
#include "Character/BossCharacter.h"

void UBossAnimInstance::AnimNotify_StartMovement()
{
	if (!FSMComp)
	{
		FSMComp = GetOwningActor()->GetComponentByClass<UFSMComponent>();
		if (!FSMComp) return;
	}

	FSMComp->StartMovement();
}

void UBossAnimInstance::AnimNotify_StopMovement()
{
	if (!FSMComp)
	{
		FSMComp = GetOwningActor()->GetComponentByClass<UFSMComponent>();
		if (!FSMComp) return;
	}

	FSMComp->StopMovement();
}

void UBossAnimInstance::AnimNotify_StartAttack()
{
	if (!BossCharacter)
	{
		BossCharacter = Cast<ABossCharacter>(GetOwningActor());
		if (!BossCharacter) return;
	}

	BossCharacter->BeginAttack();
}

void UBossAnimInstance::AnimNotify_StartAttack_Arrow()
{
	if (!BossCharacter)
	{
		BossCharacter = Cast<ABossCharacter>(GetOwningActor());
		if (!BossCharacter) return;
	}
	BossCharacter->FireArrow();
}

void UBossAnimInstance::AnimNotify_StartAttack_ArrowHard()
{
	if (!BossCharacter)
	{
		BossCharacter = Cast<ABossCharacter>(GetOwningActor());
		if (!BossCharacter) return;
	}
	BossCharacter->FireArrowHard();
}

void UBossAnimInstance::AnimNotify_Transition()
{
	if (!BossCharacter)
	{
		BossCharacter = Cast<ABossCharacter>(GetOwningActor());
		if (!BossCharacter) return;
	}
	if (!FSMComp)
	{
		FSMComp = GetOwningActor()->GetComponentByClass<UFSMComponent>();
		if (!FSMComp) return;
	}

	int val = FMath::RandRange(0, 9);
	if (val <= 9) // if player is not dead, continue combo
	{
		UE_LOG(LogTemp, Warning, TEXT("Num %d"), val);
		FName Section = Montage_GetCurrentSection();
		FString SectionS = Section.ToString();
		int32 num = FCString::Atoi(*SectionS);
		num++;
		SectionS = FString::FromInt(num);

		if (/*Is target within attack range*/true)
		{
			//Set SectionS for in-place anim sequence
		}
		else
		{
			//Set SectionS for play root anim sequence
		}	
		
		SectionS.Append("_r");
		Montage_JumpToSection(FName(SectionS), GetCurrentActiveMontage());
	}
}
