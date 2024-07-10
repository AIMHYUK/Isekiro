// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ISEKIRO_API IIDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool CanTakeDamage() = 0;
	//Response after other entity fails to give damage
	virtual void RespondToDamageFailed() = 0;
};
