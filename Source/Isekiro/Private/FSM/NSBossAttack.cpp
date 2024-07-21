// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/NSBossAttack.h"
#include "Character/BossCharacter.h"

UNSBossAttack::UNSBossAttack()
{
	bPerilousAttack = false;
	PerilousAttackType = EPerilousAttackType::NONE;
}

void UNSBossAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	auto Character = Cast<ABossCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		bPerilousAttack ? Character->BeginPerilousAttack(PerilousAttackType) : Character->BeginAttack();
	}
}

void UNSBossAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UNSBossAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	auto Character = Cast<ABossCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		Character->EndAttack();
	}
}
