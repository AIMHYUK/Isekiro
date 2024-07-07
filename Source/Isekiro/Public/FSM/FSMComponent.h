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
	RUN UMETA(DisplayName = "Run"),
	DODGE UMETA(DisplayName = "Dodge"),
	LUNGEATTACK UMETA(DisplayName = "LungeAttack"),
	JUMPATTACK UMETA(DisplayName = "JumpAttack"),
	DODGEATTACK UMETA(DisplayName = "DodgeAttack"),
	DISTANCEATTACK UMETA(DisplayName = "DistanceAttack"),
	PARRY UMETA(DisplayName = "Parry"),
	THRUST UMETA(DisplayName = "Thrust"),
	PATTERN UMETA(DisplayName = "Pattern"),
	COMBOATTACK UMETA(DisplayName = "ComboAttack"),
	ATTACK UMETA(DisplayName = "Attack")	
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

	UPROPERTY(EditAnywhere, Category = "Settings")
	EBossState StateToStart;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<EBossState, TSubclassOf<UStateObject>> BossStateMap;

private:
	void PrepNewState(EBossState NewState);
	UPROPERTY()
	TObjectPtr<UStateObject> CurrentState;
};
