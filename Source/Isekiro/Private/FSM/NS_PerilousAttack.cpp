// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/NS_PerilousAttack.h"
#include "Character/BossCharacter.h"

void UNS_PerilousAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		auto Boss = Cast<ABossCharacter>(MeshComp->GetOwner());
		if (Boss)
		{
		}
	}
}

void UNS_PerilousAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UNS_PerilousAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		auto Boss = Cast<ABossCharacter>(MeshComp->GetOwner());
		if (Boss)
		{
			Boss->EndAttack();
		}
	}
}