// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/BossAnimInstance.h"
#include "FSM/FSMComponent.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"

UBossAnimInstance::UBossAnimInstance()
{
	bIsDead = false;
}

void UBossAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	FSMComp = GetOwningActor()->GetComponentByClass<UFSMComponent>();
	BossCharacter = Cast<ABossCharacter>(GetOwningActor());
	StatusComp = BossCharacter->GetComponentByClass<UStatusComponent>();
}

void UBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (StatusComp)
	{
		bIsDead = StatusComp->IsOfficiallyDead();
	}
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

	BossCharacter->IsWithinTarget() ? FSMComp->StartMovement() : FSMComp->StopMovement();
	auto TargetStatus = BossCharacter->GetTarget()->GetComponentByClass<UStatusComponent>();
	if (TargetStatus && TargetStatus->HasHealth()) // if player is not dead, continue combo
	{
		FName Section = Montage_GetCurrentSection();
		FString SectionS = Section.ToString();
		int32 num = FCString::Atoi(*SectionS);
		num++;
		SectionS = FString::FromInt(num);

		if (!BossCharacter->IsWithinTarget())
			SectionS.Append("_r"); // play root anim sequence

		Montage_JumpToSection(FName(SectionS), GetCurrentActiveMontage());
	}
}

void UBossAnimInstance::AnimNotify_RemoveALifePoint()
{
	auto Status = GetOwningActor()->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		Status->RemoveOneLifePoint();
		Status->SetHealth(0.f);
	}
}

void UBossAnimInstance::AnimNotify_RestorePosture()
{
	auto Status = GetOwningActor()->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		Status->SetPosture(.4f);
	}
}

void UBossAnimInstance::AnimNotify_RestoreHealth()
{
	auto Status = GetOwningActor()->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		Status->SetHealth(Status->MaxHealth);
	}
}
