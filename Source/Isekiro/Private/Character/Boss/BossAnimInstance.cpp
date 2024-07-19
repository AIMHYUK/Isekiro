// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/BossAnimInstance.h"
#include "FSM/FSMComponent.h"
#include "Character/BossCharacter.h"
#include "HeroCharacter.h"
#include "ActorComponents/StatusComponent.h"
#include "IsekiroGameModeBase.h"
#include "Kismet/GameplayStatics.h"

UBossAnimInstance::UBossAnimInstance()
{
	bIsDead = false;
	bDisplayingExecutionWidget = false;
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

		FString SectionRoot = SectionS;

		if (!BossCharacter->IsWithinTarget())
			SectionRoot.Append("_r"); // play root anim sequence


		auto what = GetCurrentActiveMontage();
		if (what->IsValidSectionName(FName(SectionRoot)))
			Montage_JumpToSection(FName(SectionRoot));
		else Montage_JumpToSection(FName(SectionS));
	}
}

void UBossAnimInstance::AnimNotify_RemoveALifePoint()
{
	auto Status = GetOwningActor()->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		Status->RemoveOneLifePoint();
		Status->SetHealth(0.f);
		
		Hero = Cast<AHeroCharacter>(BossCharacter->GetTarget());
		if (Hero)
		{
			Hero->KillLifePoint();
		}
	}
}

void UBossAnimInstance::AnimNotify_Restore()
{
	auto Status = GetOwningActor()->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		if (Status->GetPosture() >= Status->MaxPosture)
			Status->SetPosture(Status->MaxPosture * .4);
		else if (Status->GetHealth() <= 0.f)
			Status->SetHealth(.01f);
	}
}

void UBossAnimInstance::AnimNotify_RestoreHealth()
{
	auto Status = GetOwningActor()->GetComponentByClass<UStatusComponent>();
	if (Status)
	{
		Status->SetHealth(Status->MaxHealth);
		Status->SetPosture(0.f);
	}
}

void UBossAnimInstance::AnimNotify_Respond()
{
	if (FSMComp)
	{
		FSMComp->RespondToInput();
	}
}

void UBossAnimInstance::AnimNotify_DeclareDead()
{
	if (!bDisplayingExecutionWidget)
	{
		bDisplayingExecutionWidget = true;
		auto GM = GetWorld()->GetAuthGameMode<AIsekiroGameModeBase>();
		if (GM) GM->GameHasEnded();
	}
}

void UBossAnimInstance::AnimNotify_EquipKatana()
{
	if (BossCharacter)
	{
		BossCharacter->EquipKatana();
	}
}

void UBossAnimInstance::AnimNotify_EquipBow()
{
	if (BossCharacter)
	{
		BossCharacter->EquipBow();
	}
}
