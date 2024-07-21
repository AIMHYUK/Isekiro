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
class UStatusComponent;

UCLASS()
class ISEKIRO_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public: 
	UBossAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	class AHeroCharacter* Hero;
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

	UFUNCTION()
	void AnimNotify_Transition();
	UFUNCTION()
	void AnimNotify_RemoveALifePoint();

	UFUNCTION()
	void AnimNotify_Restore();
	UFUNCTION()
	void AnimNotify_RestoreHealth();

	UFUNCTION()
	void AnimNotify_Respond();
	UFUNCTION()
	void AnimNotify_DeclareDead();

	UFUNCTION()
	void AnimNotify_EquipKatana();

	UFUNCTION()
	void AnimNotify_EquipBow();

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;
	bool bDisplayingExecutionWidget;
private:
	TObjectPtr<UFSMComponent> FSMComp;
	TObjectPtr<ABossCharacter> BossCharacter;
	TObjectPtr<UStatusComponent> StatusComp;
};
