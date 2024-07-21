// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "AN_PlaySound.generated.h"


enum class EBossDialogue : uint8;

UCLASS()
class ISEKIRO_API UAN_PlaySound : public UAnimNotify_PlaySound
{
	GENERATED_BODY()
	
public:
	UAN_PlaySound();
protected:
	UPROPERTY(EditAnywhere, Category="Settings")
	bool bIsDialogue;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	EBossDialogue DialogueType; 

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
