// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/BossAnimInstance.h"
#include "FSM/FSMComponent.h"
#include "Character/BossCharacter.h"

void UBossAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	FSMComp = GetOwningActor()->GetComponentByClass<UFSMComponent>();
	BossCharacter = Cast<ABossCharacter>(GetOwningActor());
}

void UBossAnimInstance::AnimNotify_StartMovement()
{
	if (!FSMComp) return;

	FSMComp->StartMovement();
}

void UBossAnimInstance::AnimNotify_StopMovement()
{
	if (!FSMComp) return;

	FSMComp->StopMovement();
}

void UBossAnimInstance::AnimNotify_StartAttack()
{
	if (!BossCharacter) return;

	BossCharacter->BeginAttack();
}

void UBossAnimInstance::AnimNotify_StartAttack_Arrow()
{
	if (!BossCharacter) return;

	BossCharacter->FireArrow();
}

void UBossAnimInstance::AnimNotify_StartAttack_ArrowHard()
{
	if (!BossCharacter) return;

	BossCharacter->FireArrowHard();
}

void UBossAnimInstance::AnimNotify_Transition()
{
	if (!BossCharacter) return;
	if (!FSMComp) return;

	BossCharacter->IsWithinTargetOffsetBuffer() ? FSMComp->StartMovement() : FSMComp->StopMovement();

	int val = FMath::RandRange(0, 9);
	val = 1;
	if (val <= 9) // if player is not dead, continue combo
	{
		UE_LOG(LogTemp, Warning, TEXT("Num %d"), val);
		FName Section = Montage_GetCurrentSection();
		FString SectionS = Section.ToString();
		int32 num = FCString::Atoi(*SectionS);
		num++;
		SectionS = FString::FromInt(num);

		if (!BossCharacter->IsWithinTargetOffsetBuffer())
			SectionS.Append("_r"); // play root anim sequence

		Montage_JumpToSection(FName(SectionS), GetCurrentActiveMontage());
	}
}
