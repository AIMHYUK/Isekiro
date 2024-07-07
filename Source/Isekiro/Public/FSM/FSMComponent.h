		// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FSMComponent.generated.h"

class UStateObject;
class ABossCharacter;

USTRUCT()
struct FStateDistance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Distance")
	float Min;
	UPROPERTY(EditDefaultsOnly, Category = "Distance")
	float Max;
};

UENUM(Blueprintable)
enum class EBossState : uint8
{
	NONE UMETA(DisplayName = "None"),
	STRAFE UMETA(DisplayName = "Strafe"),
	RUN UMETA(DisplayName = "Run"),
	DODGE UMETA(DisplayName = "Dodge"),
	HIT UMETA(DisplayName = "Hit"),
	PARRY UMETA(DisplayName = "Parry"),
	ATTACK UMETA(DisplayName = "Attack"),
	PATTERNATTACK UMETA(DisplayName = "PatternAttack"),
	DISTANCEATTACK UMETA(DisplayName = "DistanceAttack"),
	THRUSTATTACK UMETA(DisplayName = "ThrustAttack"),
	DODGEATTACK UMETA(DisplayName = "DodgeAttack"),
	JUMPATTACK UMETA(DisplayName = "JumpAttack"),
	LUNGEATTACK UMETA(DisplayName = "LungeAttack"),
	MAX UMETA(DisplayName = "Max")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISEKIRO_API UFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFSMComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	EBossState RandomState();
	
	bool IsCurrentStateActive() const;

	bool CanChangeStateTo(EBossState StateToTest);
	UFUNCTION(BlueprintCallable)
	void ChangeStateTo(EBossState NewState);
	
	void StartMovement();
	void StopMovement();
	
	EBossState GetCurrentStateE() const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	EBossState CurrentStateE;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<EBossState, TSubclassOf<UStateObject>> BossStateMap;

private:
	bool PrepNewState(EBossState NewState);
	void SetFSMState(EBossState _CurrentStateE);
	UPROPERTY()
	TObjectPtr<UStateObject> CurrentState;

	TObjectPtr<AActor> Target;
	TObjectPtr<ABossCharacter> BossCharacter;
	int32 count;
};
