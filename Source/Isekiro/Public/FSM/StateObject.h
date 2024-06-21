// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ISEKIRO_API UStateObject : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize();
	void TestFunction();
protected:
	virtual UWorld* GetWorld() const override;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<AActor> BossCharacterClass;
};
