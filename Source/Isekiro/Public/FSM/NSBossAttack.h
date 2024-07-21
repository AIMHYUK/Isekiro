// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NSBossAttack.generated.h"

/**
 *
 */
enum class EPerilousAttackType : uint8;
UCLASS()
class ISEKIRO_API UNSBossAttack : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UNSBossAttack();

	UPROPERTY(EditAnywhere)
	bool bPerilousAttack;
	UPROPERTY(EditAnywhere)
	EPerilousAttackType PerilousAttackType;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
		virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
