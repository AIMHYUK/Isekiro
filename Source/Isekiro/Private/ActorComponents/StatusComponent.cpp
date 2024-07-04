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
	StartPostureRecovery();
}

void UStatusComponent::StartPostureRecovery()
{

	GetWorld()->GetTimerManager().SetTimer(PostureRecoveryTimerHandle, this, &UStatusComponent::RecoverPosture, 1.0f, true);
}


void UStatusComponent::StopPostureRecovery()
{
	GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryTimerHandle);
}

void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UStatusComponent::ApplyPostureDamage(float Damage)
{
	if (ensureAlways(Damage > 0.f)) 
	{
		Posture += Damage;
		OnStatusChanged.Broadcast(Health, Posture);
	}
}

void UStatusComponent::ApplyHealthDamage(float Damage)
{
	if (ensureAlways(Damage > 0.f))
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
float UStatusComponent::GetPosturePercent()
{
	GetPosture();
	AGameModeBase* CurrentMode = GetWorld()->GetAuthGameMode();//현재 게임모드 가져오기
	AIsekiroGameModeBase* CurrentGameMode = Cast<AIsekiroGameModeBase>(CurrentMode); //게임모드 클래스로 변환

	if (Posture != 0)
	{
		return Posture / 100; //나중에 수정
	}
	return 0;
		
}
void UStatusComponent::RecoverPosture()
{
	// 초당 회복 속도에 따라 posture 회복
	float DeltaPosture = PostureRecoveryPerSecond * GetWorld()->GetDeltaSeconds();
	Posture -= DeltaPosture;
	if (Posture <= 0.0f)
	{
		Posture = 0.0f;
		StopPostureRecovery();
	}
}

void UStatusComponent::SetPostureZero()
{
	Posture = 0;
}
