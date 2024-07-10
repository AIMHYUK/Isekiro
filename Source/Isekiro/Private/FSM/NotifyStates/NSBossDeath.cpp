// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/NotifyStates/NSBossDeath.h"
#include "FSM/FSMComponent.h"

void UNSBossDeath::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	auto FSM = MeshComp->GetOwner()->

	if (FSMComp)
	{
		FSMComp->SetPostureState(EPostureState::BROKEN);
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
	if (Instigator)
	{
		if (FSMComp)
		{
			FSMComp->SetPostureState(EPostureState::STABLE);
		}
	}
}
