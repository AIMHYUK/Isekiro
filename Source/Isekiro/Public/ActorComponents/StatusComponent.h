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

protected:
	virtual void BeginPlay() override;

private:	
	float Health; 
	float Posture;
		
};
