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
