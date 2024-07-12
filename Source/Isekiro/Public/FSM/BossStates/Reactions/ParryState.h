// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/BossStates/Reactions/ReactionState.h"
#include "ParryState.generated.h"

/**
 *
 */
UCLASS()
class ISEKIRO_API UParryState : public UReactionState
{
	GENERATED_BODY()
public:
	UParryState();
	virtual void Start() override;
	virtual void RespondToInput() override;
};