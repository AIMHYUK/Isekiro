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
class UFSMComponent;

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

	void Initialize(UFSMComponent* FSMComp, ABossCharacter* _Instigator, AActor* _Target);
	
	virtual void Start();
	virtual EBossState Update(float DeltaTime);
	virtual void Stop();
	virtual void Activate();

	virtual void StartMovement();
	void StopMovement();
	EBossState GetFSMState() const;
	FStateDistance GetStateDistance() const;

	virtual void RespondToInput();

	UAnimMontage* GetMontage() const;

protected:
	virtual UWorld* GetWorld() const override;
	

protected:
	UAnimInstance* GetAnimInstance() const;

	void JumpToSection(FName SectionName);
	void PlayMontage();
	void StopMontage();
	virtual bool CanStartMovement() const;
	virtual EBossState UpdateMovement(float DeltaTime);

	TObjectPtr<ABossCharacter> Instigator;
	TObjectPtr<AActor> Target;
	TObjectPtr<UFSMComponent> FSMComp;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TArray<FMontageState> MontageStates;
	//Distances at which states are triggered
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FStateDistance StateDistance;
	// How far entity moves
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float TravelDist; 
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	EBossState FSMState;

private:
	bool bIsMoving;
	int32 SelectedIndex;
};
