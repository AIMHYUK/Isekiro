// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/StatusComponent.h"
#include "IsekiroGameModeBase.h"
#include "Character/IDamageInterface.h"
#include "FSM/FSMComponent.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Health = 100;
	Posture = 0;
	MaxPosture = 100;
	MaxHealth = Health;
	Portion = 3;
	MaxPortion = 3;
}


void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatusComponent::SetPosture(float Value)
{
	Posture = Value;
}

void UStatusComponent::SetHealth(float Value)
{
	Health = Value;
}

void UStatusComponent::StopPostureRecovery()
{
	GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryTimerHandle);
}

void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UStatusComponent::TryApplyDamage(float PostureDmg, float HealthDmg)
{
	if (!GetOwner()) return false;

	auto FSM = GetOwner()->GetComponentByClass<UFSMComponent>();
	if (FSM && FSM->CanStun())
	{
		if (!FSM->CanTakeDamage())
		{
			return false;
		}
	}

	ApplyDamage(PostureDmg, HealthDmg);

	return true;
}

void UStatusComponent::ApplyDamage(float PostureDmg, float HealthDmg)
{
	if (ensureAlways(PostureDmg >= 0.f && HealthDmg >= 0.f))
	{
		float OldPosture = Posture;
		float OldHealth = Health;

		Posture += PostureDmg;
		Health -= HealthDmg;

		OnStatusChanged.Broadcast(OldHealth, OldPosture, Health, Posture);
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
		return Posture / MaxPosture; //나중에 수정
	}
	return 0;

}
float UStatusComponent::GetPortion() const
{
	return Portion;
}

float UStatusComponent::GetMaxPortion() const
{
	return MaxPortion;
}

void UStatusComponent::RecoverPosture(float PostureRecoveryPerSecond)
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

void UStatusComponent::UsePortion()
{
	Portion--;
	Health += 30;
}
