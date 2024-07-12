// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/BossStates/Reactions/ReactionState.h"
#include "HitState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UHitState : public UReactionState
{
	GENERATED_BODY()
public:
	UHitState();
	virtual void Start() override;
};
