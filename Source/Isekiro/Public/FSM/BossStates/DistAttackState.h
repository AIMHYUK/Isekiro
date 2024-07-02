// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/StateObject.h"
#include "DistAttackState.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API UDistAttackState : public UStateObject
{
	GENERATED_BODY()
public:
	virtual void Start() override;
	virtual EBossState Update(float DeltaTime) override;
	virtual void Stop() override;
};
