// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/AnimNotify/AN_PlaySound.h"
#include "FSM/FSMComponent.h"

UAN_PlaySound::UAN_PlaySound()
{
	bIsDialogue = false;
	DialogueType = EBossDialogue::NONE;
}

void UAN_PlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		auto FSM = MeshComp->GetOwner()->GetComponentByClass<UFSMComponent>();
		if (FSM)
		{
			if (bIsDialogue) FSM->PlayBossSound(DialogueType);
			else if (FSM->IsDialogueActive()) return;
			else Super::Notify(MeshComp, Animation, EventReference);
		}
	}	
}
