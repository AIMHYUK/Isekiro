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
class ABossCharacter;

USTRUCT()
struct FMontageState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Settings")
	FName SectionName = "";
};

UCLASS(Blueprintable)
class ISEKIRO_API UStateObject : public UObject
{
	GENERATED_BODY()
	
public:
	UStateObject();

	void Initialize(ABossCharacter* _Instigator, AActor* _Target);
	
	virtual void Start();
	virtual EBossState Update(float DeltaTime);
	virtual void Stop();
	virtual void Activate();

	virtual void StartMovement();
	void StopMovement();

protected:
	virtual UWorld* GetWorld() const override;
	
protected:
	void PlayMontage();
	virtual bool CanStartMovement() const;
	virtual EBossState UpdateMovement(float DeltaTime);

	TObjectPtr<ABossCharacter> Instigator;
	TObjectPtr<AActor> Target;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FMontageState MontageState;
private:
	bool bIsMoving;
};
