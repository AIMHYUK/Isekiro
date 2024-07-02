		// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FSMComponent.generated.h"

class UStateObject;

UENUM(Blueprintable)
enum class EBossState : uint8
{
	NONE UMETA(DisplayName = "None"),
	STRAFE UMETA(DisplayName = "Strafe"),
	LUNGE UMETA(DisplayName = "Lunge"),
	RUN UMETA(DisplayName = "Run"),
	RUSH UMETA(DisplayName = "Rush"),
	DODGE UMETA(DisplayName = "Dodge"),
	JUMP UMETA(DisplayName = "Jump"),
	DODGEATTACK UMETA(DisplayName = "DodgeAttack"),
	DISTANCEATTACK UMETA(DisplayName = "DistanceAttack")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISEKIRO_API UFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFSMComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void ChangeStateTo(EBossState NewState);
	void StartMovement();
	void StopMovement();

protected:
	virtual void BeginPlay() override;
	
	TObjectPtr<AActor> Target;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	EBossState StateToStart;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TMap<EBossState, TSubclassOf<UStateObject>> BossStateMap;

private:
	void PrepNewState(EBossState NewState);
	UPROPERTY()
	TObjectPtr<UStateObject> CurrentState;
};
