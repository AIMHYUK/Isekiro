// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "FSM/GlobalTypes.h"
#include "BossCharacter.generated.h"

/**
 * 
 */

class UFSMComponent;
class UStateObject;
class UBoxComponent;
class AArrow;

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
	void FireArrow();
	UFUNCTION(BlueprintCallable)
	void FireArrowHard();

	float GetDistanceToTarget() const;
	FVector GetTargetOffsetLocation() const;
	bool IsLockedOnTarget() const;
	EDirection GetCurrentDirection() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UFSMComponent> FSMComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UBoxComponent> AttackBoxComp;
	UFUNCTION()
	void OnAttackBoxOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditInstanceOnly, Category = "Setting|Target")
	TObjectPtr<AActor> Target;
	UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float TargetOffset;
	UPROPERTY(EditAnywhere, Category = "Setting|Target")
	EDirection CurrDir;
	UPROPERTY(EditAnywhere, Category = "Setting|Arrow")
	TSubclassOf<AArrow> ArrowClass;
	UPROPERTY(EditAnywhere, Category = "Setting|Arrow")
	float ArrowDamage;
	UPROPERTY(EditAnywhere, Category = "Setting|Arrow")
	float ArrowHardDamage;
private:
	float HeightZ;
	void SetupFireArrow(float Damage);
};
