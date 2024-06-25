// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM/FSMComponent.h"
#include "StateObject.generated.h"

/**
 * 
 */
class AActor;

UCLASS(Blueprintable)
class ISEKIRO_API UStateObject : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(AActor* _Instigator, AActor* _Target);
	
	virtual void Start();
	virtual EBossState Update(float DeltaTime);
	virtual void Stop();
	virtual void Activate();
protected:
	virtual UWorld* GetWorld() const override;
	AActor* Instigator;
	AActor* Target;
};
