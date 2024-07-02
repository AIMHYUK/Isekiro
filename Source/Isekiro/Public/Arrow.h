// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class AActor;
class USphereComponent;
class UStaticMeshComponent;
UCLASS()
class ISEKIRO_API AArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	AArrow();
	void Initialize(AActor* _Target);

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USphereComponent> SphereComp;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UStaticMeshComponent> SMComp;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float Speed;
		
private:
	TObjectPtr<AActor> Target;
	FVector Dir;
};
