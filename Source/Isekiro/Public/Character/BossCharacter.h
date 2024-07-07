// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "FSM/GlobalTypes.h"
#include "Arrow.h"
#include "BossCharacter.generated.h"

/**
 * 
 */

class UFSMComponent;
class UStateObject;
class UBoxComponent;
class UCapsuleComponent;

UCLASS()
class ISEKIRO_API ABossCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ABossCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void BeginAttack();
	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	void FireArrow();
	UFUNCTION(BlueprintCallable)
	void FireArrowHard();
	
	AActor* GetTarget() const; 
	//Following Target location
	FVector GetTargetOffsetLocation() const;

	float GetDistanceToTargetOffset() const;
	FVector GetDirectionVectorToTarget() const;
	FVector GetNewMovementLocation(float DistanceToTravel) const;
	EDirection GetCurrentDirection() const;

	UFUNCTION(BlueprintCallable)
	bool IsWithinAttackRange() const; // not in use
	bool IsWithinTarget() const;
	bool IsWithinTarget(FVector Location) const;

	bool IsLockedOnTarget() const;
	void SetLockOnTarget(bool _bLockOnTarget);

	UFUNCTION(BlueprintCallable)
	void StartParry();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UFSMComponent> FSMComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UBoxComponent> AttackBoxComp;
	UFUNCTION()
	void OnAttackBoxOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UCapsuleComponent> LockOnComponent;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Setting|Target")
	TObjectPtr<AActor> Target;
	UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float TargetOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float TargetOffsetBuffer;

	//UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float AttackRangeDist; // not in use

	UPROPERTY(EditAnywhere, Category = "Setting|Target")
	EDirection CurrDir;
	UPROPERTY(EditAnywhere, Category = "Setting|Arrow")
	TSubclassOf<AArrow> ArrowClass;
	UPROPERTY(EditAnywhere, Category = "Setting|Arrow")
	FArrowSetting DefaultSetting;
	UPROPERTY(EditAnywhere, Category = "Setting|Arrow")
	FArrowSetting HardSetting;
private:
	float HeightZ;
	bool bLockOnTarget;
	void SetupFireArrow(FArrowSetting Setting);
};
