// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/BossStates/Reactions/ReactionState.h"
#include "DeflectedState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UDeflectedState : public UReactionState
{
	GENERATED_BODY()
public:
	UDeflectedState();
	virtual void Start() override;
	virtual void Stop() override;
};
