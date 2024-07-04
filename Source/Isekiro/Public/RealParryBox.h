// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "RealParryBox.generated.h"

/**
 * 
 */
class UAnimInstance;
class AHeroCharacter;

UCLASS()
class ISEKIRO_API URealParryBox : public USphereComponent
{
	GENERATED_BODY()
	
public:	
	URealParryBox();
	// Sets default values for this component's properties
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*class UStatusComponent* State;*/
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	 // 충돌 이벤트 처리 함수
    UFUNCTION() //이걸해야 인식해용
    void OnParryCheckBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ParryStarted();
	void ParryEnded();	
	bool GetParryWindow();

	bool bIsParryWindow;
	FTimerHandle ParryTimerHandle;

	AHeroCharacter* MyCharacter;

	void ResetTimeDilation();

	FTimerHandle TimeDilationHandle;

	FTimerHandle CooldownTimerHandle;

	void ResetParryCooldown();

	bool bCanParry;

};
