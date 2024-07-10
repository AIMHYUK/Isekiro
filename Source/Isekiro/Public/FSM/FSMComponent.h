		// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FSMComponent.generated.h"

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
	
	HIT UMETA(DisplayName = "Hit"),							//Stun
	PARRY UMETA(DisplayName = "Parry"),						//Stun
	
	// add normal attack state								//Near

	ATTACK UMETA(DisplayName = "Attack"),					//Both
	JUMPATTACK UMETA(DisplayName = "JumpAttack"),			//Both
	THRUSTATTACK UMETA(DisplayName = "ThrustAttack"),		//Both

	STRAFE UMETA(DisplayName = "Strafe"),					//Far
	RUN UMETA(DisplayName = "Run"),							//Far
	PATTERNATTACK UMETA(DisplayName = "PatternAttack"),		//Far
	DISTANCEATTACK UMETA(DisplayName = "DistanceAttack"),	//Far
	LUNGEATTACK UMETA(DisplayName = "LungeAttack"),			//Far
		
	DODGE UMETA(DisplayName = "Dodge"),						//Transition
	DODGEATTACK UMETA(DisplayName = "DodgeAttack"),			//Transition

	DEATH UMETA(DisplayName = "Death"),						//Death

	MAX UMETA(DisplayName = "Max")
};

UENUM()
enum class EFightingSpace : uint8
{
	NEAR UMETA(DisplayName = "Near"),
	FAR UMETA(DisplayName = "Far")
};

UENUM()
enum class EPostureState : uint8
{
	STABLE,
	BROKEN
};

DECLARE_MULTICAST_DELEGATE(FStateResponseDelegate);

class UStateObject;
class ABossCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISEKIRO_API UFSMComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class UDeathState;
public:	
	UFSMComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FStateResponseDelegate OnStateResponse;

	EBossState RandomState();
	bool IsCurrentStateActive() const;
	bool CanChangeStateTo(EBossState StateToTest);
	UFUNCTION(BlueprintCallable)
	void ChangeStateTo(EBossState NewState);

	void StartMovement();
	void StopMovement();
	void RespondToState();
	
	UFUNCTION(BlueprintCallable)
	virtual void RespondToInput();

	EBossState GetCurrentStateE() const;
	bool IsPostureBroken() const;
	void SetPostureState(EPostureState _PostureState);
	bool CanTakeDamage();

	//Can player stop boss's active attacks by forcing boss to respond to player attacks?
	bool CanStun() const;
	//if parry fails, boss starts hit state.
	bool CanParry() const;
	void EnableStun(bool bStun);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<EBossState, TSubclassOf<UStateObject>> BossStateMap;

private:
	bool HandleDodgeProbability();
	void RespondToDamageTakenFailed();

	UPROPERTY(EditAnywhere, Category = "Settings", meta=(AllowPrivateAccess))
	EBossState CurrentStateE;
	EFightingSpace FightSpace;
	EPostureState PostureState;

	bool PrepNewState(EBossState NewState);
	void SetFSMState(EBossState _CurrentStateE);
	UPROPERTY()
	TObjectPtr<UStateObject> CurrentState;

	TObjectPtr<AActor> Target;
	TObjectPtr<ABossCharacter> BossCharacter;

	bool bCanStun;
	UPROPERTY(EditAnywhere, Category = "Settings|Dodge", meta=(AllowPrivateAccess))
	float DodgeMaxProb;
	UPROPERTY(EditAnywhere, Category = "Settings|Dodge", meta=(AllowPrivateAccess))
	float DodgeProbRate;
	float DodgeProbTotal;
};
