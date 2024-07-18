// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class AActor;
class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;

USTRUCT()
struct FArrowSetting
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Damage;
	UPROPERTY(EditDefaultsOnly)
	float Speed;
};

UCLASS()
class ISEKIRO_API AArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	AArrow();
	void Initialize(AActor* _Target, FArrowSetting _ArrowSetting);

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USphereComponent> SphereComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UStaticMeshComponent> SMComp;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UNiagaraComponent> ArrowTrailComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UNiagaraSystem> ArrowTrail;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	



private:
	TObjectPtr<AActor> Target;
	FVector Dir;
	FArrowSetting ArrowSetting;
	bool bHasHit;
};
