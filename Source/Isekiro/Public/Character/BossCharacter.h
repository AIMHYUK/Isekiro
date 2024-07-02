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

	float GetDistanceToTarget() const;
	FVector GetTargetOffsetLocation() const;
	bool IsLockedOnTarget() const;
	EDirection GetCurrentDirection() const;


protected:
	UFUNCTION()
	void OnAttackBoxOverlapped(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnDeactivated(UActorComponent* Component);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UFSMComponent> FSMComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UBoxComponent> AttackBoxComp;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Setting")
	TObjectPtr<AActor> Target;
	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float TargetOffset;
	UPROPERTY(EditAnywhere, Category = "Setting")
	EDirection CurrDir;
	UPROPERTY(EditAnywhere, Category = "Setting")
	TSubclassOf<AArrow> ArrowClass;

private:
	float HeightZ;
};
