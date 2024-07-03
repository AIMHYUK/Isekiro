// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/StatusComponent.h"
#include "IsekiroGameModeBase.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Health = 100;
	Posture = 0;
	MaxHealth = Health;
}


void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UStatusComponent::ApplyPostureDamage(float Damage)
{
	if (ensureAlways(Damage < 0.f)) 
	{
		Posture -= Damage;
		OnStatusChanged.Broadcast(Health, Posture);
	}
}

void UStatusComponent::ApplyHealthDamage(float Damage)
{
	if (ensureAlways(Damage < 0.f))
	{
		Health -= Damage;
		OnStatusChanged.Broadcast(Health, Posture);
	}
}

float UStatusComponent::GetHealth() const
{
	return Health;
}

float UStatusComponent::GetPosture() const
{
	return Posture;
}

float UStatusComponent::GetHPPercent()
{
	GetHealth();
	AGameModeBase* CurrentMode = GetWorld()->GetAuthGameMode();//현재 게임모드 가져오기
	AIsekiroGameModeBase* CurrentGameMode = Cast<AIsekiroGameModeBase>(CurrentMode); //게임모드 클래스로 변환

	return Health / MaxHealth;
}