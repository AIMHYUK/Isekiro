// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/NotifyStates/NSHazardAttack.h"
#include "Character/BossCharacter.h"
#include "HeroCharacter.h"

void UNSHazardAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	auto Boss = MeshComp->GetOwner<ABossCharacter>();

	if (Boss)
	{
		auto Hero = Cast<AHeroCharacter>(Boss->GetTarget());
		if (Hero)
		{
			Hero->SetActionStateHazardBegin();
		}
	}
}

void UNSHazardAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UNSHazardAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	auto Boss = MeshComp->GetOwner<ABossCharacter>();

	if (Boss)
	{
		auto Hero = Cast<AHeroCharacter>(Boss->GetTarget());
		if (Hero)
		{
			Hero->SetActionStateHazardEnd();
		}
	}
}
