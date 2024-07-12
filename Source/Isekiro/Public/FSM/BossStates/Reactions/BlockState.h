// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/BossStates/Reactions/ReactionState.h"
#include "BlockState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UBlockState : public UReactionState
{
	GENERATED_BODY()
public: 
	virtual void Start() override;
};
