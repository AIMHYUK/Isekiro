// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossAnimInstance.generated.h"

/**
 * 
 */

class UFSMComponent;
class ABossCharacter;
UCLASS()
class ISEKIRO_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	void AnimNotify_StartMovement();

	UFUNCTION()
	void AnimNotify_StopMovement();

	UFUNCTION()
	void AnimNotify_StartAttack();
	
	UFUNCTION()
	void AnimNotify_StartAttack_Arrow();

	UFUNCTION()
	void AnimNotify_StartAttack_ArrowHard();

private:
	TObjectPtr<UFSMComponent> FSMComp;
	TObjectPtr<ABossCharacter> BossCharacter;
};
