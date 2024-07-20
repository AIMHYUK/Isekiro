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

	//Stun
	HIT UMETA(DisplayName = "Hit"),							
	//Stun
	BLOCK UMETA(DisplayName = "Block"),						
	//Stun
	PARRY UMETA(DisplayName = "Parry"),						
	//Stun
	DEFLECTED UMETA(DisplayName = "Deflected"),				
				
	//Near
	NORMALATTACK UMETA(DisplayName = "NormalAttack"),
	//Near
	JUMPSTRIKE UMETA(DisplayName = "JumpStrike"),						
		
	//Both
	COUNTERATTACK UMETA(DisplayName = "CounterAttack"),
	//Both
	DISTANCEATTACK UMETA(DisplayName = "DistanceAttack"),
	//Both
	JUMPFIRE  UMETA(DisplayName = "JumpFire"),
	//Both
	STRAFE UMETA(DisplayName = "Strafe"),

	//Far
	THRUSTATTACK UMETA(DisplayName = "ThrustAttack"),
	//Far
	RUN UMETA(DisplayName = "Run"),							
	//Far
	PATTERNATTACK UMETA(DisplayName = "PatternAttack"),		
	//Far
	LUNGEATTACK UMETA(DisplayName = "LungeAttack"),			
		
	//Transition
	DODGE UMETA(DisplayName = "Dodge"),						
	//Transition
	DODGEATTACK UMETA(DisplayName = "DodgeAttack"),			

	//Death
	DEATH UMETA(DisplayName = "Death"),						

	MAX UMETA(DisplayName = "Max")
};

UENUM()
enum class EFightingSpace : uint8
{
	NEAR UMETA(DisplayName = "Near"),
	FAR UMETA(DisplayName = "Far")
};

UENUM(Blueprintable)
enum class EPostureState : uint8
{
	STABLE UMETA(DisplayName = "Stable"),
	BROKEN UMETA(DisplayName = "Broken")
};

DECLARE_MULTICAST_DELEGATE(FStateResponseDelegate);

class UStateObject;
class ABossCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISEKIRO_API UFSMComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class UDeathState;
	friend class UParryState;
	friend class UStateObject;
public:	
	UFSMComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FStateResponseDelegate OnStateResponse;

	EBossState RandomState();
	bool IsCurrentStateActive() const;
	bool TargetWithinRangeFor(EBossState BossState);
	UFUNCTION(BlueprintCallable)
	void ChangeStateTo(EBossState NewState);
	
	bool IsReactionState(EBossState State) const;
	bool IsMeleeState(EBossState State) const;
	bool CanRecoverPosture() const;

	void StartMovement();
	void StopMovement();
	UFUNCTION(BlueprintCallable)
	virtual void RespondToInput();

	EBossState GetCurrentStateE() const;
	UFUNCTION(BlueprintCallable)
	bool IsPostureBroken() const;
	UFUNCTION(BlueprintCallable)
	EPostureState GetPostureState() const;
	void SetPostureState(EPostureState _PostureState);
	bool CanTakeDamage();

	//Can player stop boss's active attacks by forcing boss to respond to player attacks?
	bool CanStun() const;
	//if parry fails, boss starts hit state.
	bool CanDefend();
	void EnableStun(bool bStun);
	void StartParryOrBlock();

	void EnableDefense(bool bEnabled);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<EBossState, TSubclassOf<UStateObject>> BossStateMap;
	
private:
	bool HandleDodgeProbability();
	void PerformDodge();

	UPROPERTY(EditAnywhere, Category = "Settings", meta=(AllowPrivateAccess))
	EBossState CurrentStateE;
	EBossState PrevStateE;
	void SetPreviousState(EBossState State);
	EFightingSpace FightSpace;
	EPostureState PostureState;

	bool PrepNewState(EBossState NewState);
	void SetFSMState(EBossState _CurrentStateE);
	UPROPERTY()
	TObjectPtr<UStateObject> CurrentState;

	TObjectPtr<AActor> Target;
	TObjectPtr<ABossCharacter> BossCharacter;

	bool bCanStun;
	float DodgeMaxProb;
	/*Rate at which probability of performing dodge or dodgeAttack increases. 
	Probability increases everytime Player attacks. 
	Probability is out of 100.*/
	UPROPERTY(EditAnywhere, Category = "Settings|Dodge", meta=(AllowPrivateAccess), meta=(ClampMin="0.0"), meta=(ClampMax="30.0"))
	float DodgeProbRate;
	float DodgeProbTotal;

	//Default probability of performing defensive measures against player's attacks.
	UPROPERTY(EditAnywhere, Category = "Settings|Defense", meta = (AllowPrivateAccess), meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float DefenseProbability;
	//Probability of blocking player's attack during the boss's Deflected State.
	UPROPERTY(EditAnywhere, Category = "Settings|Defense", meta = (AllowPrivateAccess), meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float DeflectedBlockProb;
	float DefenseProb;



	//Probability of parrying attacks. Performs Block otherwise.
	UPROPERTY(EditAnywhere, Category = "Settings|Defense", meta = (AllowPrivateAccess), meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float ParryProbability;
};
