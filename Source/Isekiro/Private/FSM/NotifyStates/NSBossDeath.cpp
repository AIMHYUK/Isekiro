// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/NotifyStates/NSBossDeath.h"
#include "FSM/FSMComponent.h"
#include "Character/BossCharacter.h"
#include "BossWidget.h"

void UNSBossDeath::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		auto FSM = MeshComp->GetOwner()->GetComponentByClass<UFSMComponent>();
		if (FSM)
		{
			FSM->SetPostureState(EPostureState::BROKEN);
		}
	}
	//change posture state to BROKEN
}

void UNSBossDeath::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UNSBossDeath::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		auto Boss = MeshComp->GetOwner<ABossCharacter>();
		if (Boss)
		{
			auto FSM = Boss->GetComponentByClass<UFSMComponent>();
			if (FSM)
			{
				if (Boss->GetBossUI()) Boss->GetBossUI()->DisplayPostureBroken(false);
				FSM->SetPostureState(EPostureState::STABLE);
			}
		}
	}
}
