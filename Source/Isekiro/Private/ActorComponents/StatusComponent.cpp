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
	Portion = 10;
	MaxPortion = 10;
	LifePoints = 2;
	bIsDead = false;
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

bool UStatusComponent::TryApplyDamage(AActor* Initiator, float PostureDmg, float HealthDmg)
{
	if (!GetOwner()) return false;

	auto Status = GetOwner()->GetComponentByClass<UStatusComponent>();
	if (Status && !Status->HasHealth())
	{
		return false;
	}

	auto FSM = GetOwner()->GetComponentByClass<UFSMComponent>();
	if (FSM)
	{
		if (FSM->GetCurrentStateE() == EBossState::DEATH)
		{
			return false;
		}
		else if(FSM->CanStun())
		{
			if (!FSM->CanTakeDamage())
			{
				return false;
			}
		}
	}

	ApplyDamage(Initiator, PostureDmg, HealthDmg);

	return true;
}

void UStatusComponent::ApplyDamage(AActor* Initiator, float PostureDmg, float HealthDmg)
{
	if (ensureAlways(PostureDmg >= 0.f && HealthDmg >= 0.f))
	{
		float OldPosture = Posture;
		float OldHealth = Health;

		Posture += PostureDmg;
		Health -= HealthDmg;

		OnStatusChanged.Broadcast(Initiator, OldHealth, OldPosture, Health, Posture);
	}
}

bool UStatusComponent::IsPostureBroken() const
{
	return Posture >= MaxPosture;
}

bool UStatusComponent::HasHealth() const
{
	return Health > 0.f;
}

bool UStatusComponent::IsAlive() const
{
	return LifePoints > 0;
}

float UStatusComponent::GetHealth() const
{
	return Health;
}

float UStatusComponent::GetPosture() const
{
	return Posture;
}

int UStatusComponent::GetLifePoints() const
{
	return LifePoints;
}

void UStatusComponent::RemoveOneLifePoint()
{
	--LifePoints;
}

void UStatusComponent::OfficiallyDeclareDead()
{
	bIsDead = true;
}

bool UStatusComponent::IsOfficiallyDead() const
{
	return bIsDead;
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
