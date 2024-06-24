// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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
	virtual UStateObject* Update(float DeltaTime);
	virtual void Stop();
protected:
	virtual UWorld* GetWorld() const override;
	AActor* Instigator;
	AActor* Target;
};
