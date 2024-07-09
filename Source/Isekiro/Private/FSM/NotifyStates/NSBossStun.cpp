// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/NotifyStates/NSBossStun.h"
#include "FSM/FSMComponent.h"

void UNSBossStun::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	auto FSMComp = MeshComp->GetOwner()->GetComponentByClass<UFSMComponent>();
	if (FSMComp)
	{
		FSMComp->EnableStun(false);
	}
}

void UNSBossStun::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UNSBossStun::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	auto FSMComp = MeshComp->GetOwner()->GetComponentByClass<UFSMComponent>();
	if (FSMComp)
	{
		FSMComp->EnableStun(true);
	}
}
