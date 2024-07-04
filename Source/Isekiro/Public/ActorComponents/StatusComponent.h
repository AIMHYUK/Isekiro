// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusChangedDelegate, float, Health, float, Posture);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISEKIRO_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatusComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ApplyPostureDamage(float Damage);
	void ApplyHealthDamage(float Damage);

	float GetHealth() const;
	float GetPosture() const;
	FStatusChangedDelegate OnStatusChanged; 

	UFUNCTION(BlueprintCallable)
	float GetHPPercent();
	UFUNCTION(BlueprintCallable)
	float GetPosturePercent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    float PostureRecoveryPerSecond = 5.0f; // 초당 회복되는 posture 양

	UFUNCTION(BlueprintCallable)
	void StartPostureRecovery();
	UFUNCTION(BlueprintCallable)
	void StopPostureRecovery();

	float CurrentPosture; // 현재 posture 값
	FTimerHandle PostureRecoveryTimerHandle;

	UFUNCTION(BlueprintCallable)
	void RecoverPosture(); // posture를 회복하는 함수

	void SetPostureZero();
protected:
	virtual void BeginPlay() override; // BeginPlay에서 자연 회복 시작

private:	
	float Health; 
	float Posture;
	float MaxHealth;
		

	

};
