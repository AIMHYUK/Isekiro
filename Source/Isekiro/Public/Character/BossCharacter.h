// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "FSM/GlobalTypes.h"
#include "Arrow.h"
#include "IDamageInterface.h"
#include "BossCharacter.generated.h"

/**
 * 
 */

class UFSMComponent;
class UStateObject;
class UBoxComponent;
class UCapsuleComponent;
class UBossWidget;
class UWidgetComponent;

UCLASS()
class ISEKIRO_API ABossCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ABossCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

public:
	UBossWidget* GetBossUI() const;

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
	FVector GetTargetLoc() const;
	float GetTargetOffset() const;

	float GetDistanceToTargetOffset() const;
	float GetDistanceToTarget() const;
	FVector GetDirectionVectorToTarget() const;
	FVector GetDirectionVector(EDirection Dir) const;
	FVector GetNewMovementLocation(float DistanceToTravel, EDirection Dir) const;
	EDirection GetCurrentDirection() const;

	UFUNCTION(BlueprintCallable)
	bool IsWithinNearRange() const;
	bool IsWithinTarget() const;
	bool IsWithinTarget(FVector Location, float Offset) const;

	bool IsLockedOnTarget() const;
	void SetLockOnTarget(bool _bLockOnTarget);
	void ResetHeight();

	UFUNCTION(BlueprintImplementableEvent)
	void EquipKatana();
	UFUNCTION(BlueprintImplementableEvent)
	void EquipBow();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Components")
	TObjectPtr<USkeletalMeshComponent> RetargetedSKMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Components")
	TObjectPtr<UStaticMeshComponent> WeaponSMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Components")
	TObjectPtr<UFSMComponent> FSMComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UBoxComponent> AttackBoxComp;
	
	UFUNCTION()
	void OnAttackBoxOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCapsuleOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnStatusChanged(float OldHealth, float OldPosture, float NewHealth, float NewPosture);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UCapsuleComponent> LockOnComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|Components")
	TObjectPtr<UWidgetComponent> TargetWidgetComponent;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Setting|Target")
	TObjectPtr<AActor> Target;
	UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float TargetOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float TargetOffsetBuffer;
	UPROPERTY(EditAnywhere, Category = "Setting|Target")
	EDirection CurrDir;
	UPROPERTY(EditDefaultsOnly, Category = "Setting|Target")
	float NearSpaceBuffer;
	
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

	TObjectPtr<UBossWidget> BossUI;
	bool CanRecoverPosture() const;
};
