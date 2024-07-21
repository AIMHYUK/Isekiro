// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/BossStates/StrafeState.h"
#include "Character/BossCharacter.h"
#include "ActorComponents/StatusComponent.h"

UStrafeState::UStrafeState()
{
	StrafeSpeed = 80.f;
	StrafeMaxTime = 4.f;
	StrafeTotalTime = 0.f;
	StateDistance.Min = 250.f;
}

void UStrafeState::Start()
{
	Super::Start();
	StartMovement();

	if (FSMComp && FSMComp->HasEnteredFight())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &UStrafeState::EnteredFight, .5f, false);
		StrafeMaxTime = 2.2f;
	}
	else
		StrafeMaxTime = FMath::RandRange(std::min(StrafeMaxTime, StrafeMaxTime - 1.f), StrafeMaxTime);

	
	if (Instigator && Instigator->GetTarget())
	{
		auto Status = Instigator->GetTarget()->GetComponentByClass<UStatusComponent>();
		if (!Status->HasHealth())
		{
			int32 SecNum = FMath::RandRange(1, 2);
			JumpToSection(FName(FString::FromInt(SecNum)));
			StopMovement();
		}
	}
}

EBossState UStrafeState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	auto Status = Instigator->GetTarget()->GetComponentByClass<UStatusComponent>();
	if (!Status->HasHealth())
	{
		return EBossState::NONE;
	}

	if (StrafeTotalTime >= StrafeMaxTime && FSMComp)
	{
		if (FSMComp && FSMComp->HasEnteredFight())
		{
			FSMComp->ChangeStateTo(EBossState::RUN);
		}
		else return FSMComp->RandomState();
	}
	return EBossState::NONE;
}

void UStrafeState::Stop()
{
	Super::Stop();
}

EBossState UStrafeState::UpdateMovement(float DeltaTime)
{
	Super::UpdateMovement(DeltaTime);

	if (StrafeTotalTime < StrafeMaxTime)
	{
		StrafeTotalTime += DeltaTime;

		if (Instigator)
		{
			FVector newLoc = Instigator->GetActorLocation() + Instigator->GetActorRightVector() * StrafeSpeed * DeltaTime;
			Instigator->SetActorLocation(newLoc);
		}
	}
	return EBossState::NONE;
}

void UStrafeState::EnteredFight()
{
	if (FSMComp) FSMComp->PlayBossSound(EBossDialogue::START01);
}
