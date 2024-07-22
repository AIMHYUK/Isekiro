// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FStatusChangedDelegate, AActor*, Initiator, float, OldHealth, float, OldPosture, float, NewHealth, float, NewPosture);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISEKIRO_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ABossCharacter;
	friend class UDeathState;
	friend class UBossAnimInstance;
public:	
	UStatusComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//
	bool TryApplyDamage(AActor* Initiator, float PostureDmg, float HealthDmg);
	void ApplyDamage(AActor* Initiator, float PostureDmg, float HealthDmg);

	bool IsPostureBroken() const;
	bool HasHealth() const;
	bool IsAlive() const;

	float GetHealth() const;
	float GetPosture() const;
	UFUNCTION(BlueprintCallable)
	int GetLifePoints() const;
	void RemoveOneLifePoint();
	void OfficiallyDeclareDead();
	bool IsOfficiallyDead() const;

	FStatusChangedDelegate OnStatusChanged; 

	UFUNCTION(BlueprintCallable)
	float GetHPPercent();
	UFUNCTION(BlueprintCallable)
	float GetPosturePercent();
	UFUNCTION(BlueprintCallable)
	float GetPortion() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxPortion() const;

	UFUNCTION(BlueprintCallable)
	void StopPostureRecovery();

	float CurrentPosture; // 현재 posture 값
	FTimerHandle PostureRecoveryTimerHandle;

	UFUNCTION(BlueprintCallable)
	void RecoverPosture(float PostureRecoveryPerSecond); // posture를 회복하는 함수

	void SetPostureZero();
	void UsePortion();
protected:
	virtual void BeginPlay() override; // BeginPlay에서 자연 회복 시작

	UPROPERTY(EditAnywhere, Category="Settings")
	float MaxPosture;

private:
	float Health; 
	float Posture;
	float MaxHealth;
	float Portion;
	float MaxPortion;
	UPROPERTY(EditAnywhere, Category = "Settings", meta=(AllowPrivateAccess))
	int LifePoints;
		
	void SetPosture(float Value);
	void SetHealth(float Value);

	bool bIsDead;
};
