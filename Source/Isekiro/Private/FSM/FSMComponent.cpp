// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/FSMComponent.h"
#include "FSM/StateObject.h"
#include "FSM/BossStates/StrafeState.h"
#include "Character/BossCharacter.h"
#include "IsekiroGameModeBase.h"
#include "ActorComponents/StatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentStateE = EBossState::STRAFE;
	PrevStateE = EBossState::NONE;

	bCanStun = true;

	DodgeMaxProb = 100.f;
	DodgeProbRate = 3.f;
	DodgeProbTotal = 0.f;

	FightSpace = EFightingSpace::FAR;

	DefenseProbability = 1.f;
	DefenseProb = DefenseProbability;
	ParryProbability = .4f;

	DeflectedBlockProb = .4f;

	bHasEnteredFight = true;

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
}


void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComp)
	{
		AudioComp->OnAudioFinished.AddDynamic(this, &UFSMComponent::OnDialogueFinished);
	}

	DefenseProb = DefenseProbability;

	auto* Boss = Cast<ABossCharacter>(GetOwner());
	if (Boss)
	{
		Target = Boss->GetTarget();
		BossCharacter = Boss;
	}

	if (CurrentStateE != EBossState::NONE)
	{
		ChangeStateTo(CurrentStateE);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Starting State has not been set. Boss will not perform any actions."));
	}
}

void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(CurrentState))
	{
		EBossState NewState = CurrentState->Update(DeltaTime);
		if (NewState != EBossState::NONE)
		{
			ChangeStateTo(NewState);
		}
	}

	if (CanStun())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Yes Can Stun")));
	}
	else GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("No Can't Stun")));

	switch (FightSpace)
	{
	case EFightingSpace::FAR:
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("FAR")));
		break;
	case EFightingSpace::NEAR:
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("NEAR")));
		break;
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Magenta, FString::Printf(TEXT("CurrentState: %s"), *GetNameSafe(CurrentState)));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Magenta, FString::Printf(TEXT("PreviousState: %s"), *UEnum::GetValueAsString(PrevStateE)));
}

EBossState UFSMComponent::RandomState()
{
	if (Target)
	{
		auto Status = Target->GetComponentByClass<UStatusComponent>();
		if (Status && !Status->HasHealth())
			return EBossState::STRAFE;
	}

	int index;
	int32 min = 0;
	int32 max = 0;

	if (BossCharacter && BossCharacter->IsWithinNearRange())
		FightSpace = EFightingSpace::NEAR;
	else FightSpace = EFightingSpace::FAR;

	switch (FightSpace)
	{
	case EFightingSpace::NEAR:
		min = (int32)EBossState::NORMALATTACK;
		max = (int32)EBossState::JUMPSTRIKE;
		break;
	case EFightingSpace::FAR:
		min = (int32)EBossState::JUMPSTRIKE;
		max = (int32)EBossState::LUNGEATTACK;
		break;
	}

	int32 RandomStateCount = -1;
	do
	{
		index = FMath::RandRange(min, max);
		RandomStateCount++;
	} while (
		RandomStateCount < 50 && !TargetWithinRangeFor((EBossState)index) || EBossState(index) == PrevStateE
		);

	if (RandomStateCount >= 50)
	{
		return EBossState::STRAFE;
		/*if (BossCharacter && BossCharacter->IsWithinTarget())
		{
			if (FMath::RandRange(0, 1 == 1))
			{
				FightSpace = EFightingSpace::NEAR;
				RandomState();
			}
		}
		else
		{
			if (FMath::RandRange(0, 1 == 1))
			{
				return EBossState::STRAFE;
			}
			else return EBossState::THRUSTATTACK;
		}*/
	}

	return EBossState(index);
}

bool UFSMComponent::IsCurrentStateActive() const
{
	return BossCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
}

bool UFSMComponent::TargetWithinRangeFor(EBossState BossState)
{
	if (BossState == EBossState::NONE) return false;

	if (BossState == CurrentStateE) return false;

	TSubclassOf<UStateObject>* StateObjectClass = BossStateMap.Find(BossState);
	if (StateObjectClass && *StateObjectClass)
	{
		TSubclassOf<UStateObject> StateObjClass = *StateObjectClass;
		FStateDistance StateDist = StateObjClass->GetDefaultObject<UStateObject>()->GetStateDistance();

		if (BossCharacter)
		{
			float DistToTarget = BossCharacter->GetDistanceToTargetOffset();
			return DistToTarget >= StateDist.Min && DistToTarget <= StateDist.Max;
		}
	}
	return false;
}

void UFSMComponent::ChangeStateTo(EBossState NewState)
{

	if (PrepNewState(NewState))
	{
		SetPreviousPatternState(NewState);
		CurrentStateE = NewState;
	}
	else
	{
		SetPreviousPatternState(NewState);
		CurrentStateE = EBossState::NONE;
	}
}
bool UFSMComponent::IsReactionState(EBossState State) const
{
	return State == EBossState::DEATH ||
		State == EBossState::PARRY ||
		State == EBossState::HIT ||
		State == EBossState::DEFLECTED ||
		State == EBossState::BLOCK;
}

bool UFSMComponent::IsMeleeState(EBossState State) const
{
	return IsReactionState(State) || State == EBossState::NORMALATTACK;
}

bool UFSMComponent::CanRecoverPosture() const
{
	return CanStun() && !IsReactionState(CurrentStateE) && CurrentStateE != EBossState::NORMALATTACK;
}

void UFSMComponent::RespondToInput()
{
	if (CurrentState)
	{
		CurrentState->RespondToInput();
	}
}

void UFSMComponent::StartMovement()
{
	if (CurrentState)
	{
		CurrentState->StartMovement();
	}
}

void UFSMComponent::StopMovement()
{
	if (CurrentState)
	{
		CurrentState->StopMovement();
	}
}

EBossState UFSMComponent::GetCurrentStateE() const
{
	return CurrentStateE;
}

bool UFSMComponent::IsPostureBroken() const
{
	return PostureState == EPostureState::BROKEN;
}

EPostureState UFSMComponent::GetPostureState() const
{
	return PostureState;
}

void UFSMComponent::SetPostureState(EPostureState _PostureState)
{
	PostureState = _PostureState;
}

bool UFSMComponent::CanTakeDamage()
{
	if (HandleDodgeProbability())
	{
		PerformDodge();
		return false;
	}
	else return true;
}

bool UFSMComponent::HandleDodgeProbability()
{
	if (FMath::RandRange(0.f, 1.f) >= DodgeProbTotal / DodgeMaxProb) // Take Damage
	{
		FightSpace = EFightingSpace::NEAR;
		DodgeProbTotal += DodgeProbRate;
		return false;
	}
	else
	{
		FightSpace = EFightingSpace::FAR;
		DodgeProbTotal = 0.f;
		return true;
	}
}

void UFSMComponent::PerformDodge()
{
	switch (FMath::RandRange(0, 1))
	{
	case 0:
	{
		ChangeStateTo(EBossState::DODGEATTACK);
		break;
	}
	case 1:
		ChangeStateTo(EBossState::DODGE);
		break;
	}
}

bool UFSMComponent::CanStun() const
{
	return bCanStun;
}

void UFSMComponent::EnableStun(bool bStun)
{
	bCanStun = bStun;
}

void UFSMComponent::StartParryOrBlock()
{
	float Prob = FMath::RandRange(0.f, 1.f);
	if (Prob <= ParryProbability)
	{
		ChangeStateTo(EBossState::PARRY);
		AIsekiroGameModeBase::SpawnCollisionEffect(GetOwner(), BossCharacter->GetMesh()->GetSocketLocation(TEXT("RightHandSocketBase")),
			EWeaponCollisionType::PARRY);
	}
	else
	{
		ChangeStateTo(EBossState::BLOCK);
		AIsekiroGameModeBase::SpawnCollisionEffect(GetOwner(), BossCharacter->GetMesh()->GetSocketLocation(TEXT("RightHandSocketBase")),
			EWeaponCollisionType::BLOCK);
	}
}

void UFSMComponent::EnableDefense(bool bEnabled)
{
	bEnabled ? DefenseProb = DefenseProbability : DefenseProb = DeflectedBlockProb;
}

bool UFSMComponent::HasEnteredFight() const
{
	return bHasEnteredFight;
}

void UFSMComponent::SetHasEnteredFight(bool _bHasEnteredFight)
{
	bHasEnteredFight = _bHasEnteredFight;
}

bool UFSMComponent::IsDialogueActive() const
{
	return bDialogueActive;
}

void UFSMComponent::PlayBossSound(EBossDialogue DialogueType)
{
	if (DialogueType == EBossDialogue::NONE) return;

	auto DialogueIt = BossDialogue.Find(DialogueType);
	if (DialogueIt && *DialogueIt)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dialogue Started"));
		AudioComp->SetSound(*DialogueIt);
		AudioComp->Play();

		//UGameplayStatics::PlaySound2D(GetWorld(), *DialogueIt);
		bDialogueActive = true;
	}
}

void UFSMComponent::OnDialogueFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Dialogue Finished"));
	if(bDialogueActive) bDialogueActive = false;
}

bool UFSMComponent::CanDefend()
{
	float value = FMath::RandRange(0.f, 1.f);
	switch (CurrentStateE)
	{
	case EBossState::NONE:
		return false;
	case EBossState::STRAFE:
		return true;
	default:
	{
		if (BossCharacter)
		{
			auto* StatusComp = BossCharacter->GetComponentByClass<UStatusComponent>();
			if (StatusComp)
			{
				if (IsMeleeState(CurrentStateE))
				{
					return value <= DefenseProb;
				}
				else // Boss is performing some attack pattern that is not a melee type state.
				{
					EnableDefense(false);
					bool bDefended = value <= DefenseProb;
					EnableDefense(true);
					return bDefended;
				}
			}
		}
	}
	}

	return false;
}

bool UFSMComponent::PrepNewState(EBossState NewState)
{
	TSubclassOf<UStateObject>* StateObjectClass = BossStateMap.Find(NewState);
	if (StateObjectClass && *StateObjectClass)
	{
		UStateObject* NewStateObj = NewObject<UStateObject>(GetOwner(), *StateObjectClass);
		if (NewStateObj)
		{
			NewStateObj->Initialize(this, Cast<ABossCharacter>(GetOwner()), Target);
			if (CurrentState) CurrentState->Stop();
			NewStateObj->Start();
			CurrentState = NewStateObj;
			return true;
		}
		else UE_LOG(LogTemp, Warning, TEXT("Could not Instantiate new boss State object."));
	}
	else UE_LOG(LogTemp, Warning, TEXT("Could not find Boss State in BossStateMap."));

	return false;
}

void UFSMComponent::SetCurrentState(EBossState _CurrentStateE)
{
	CurrentStateE = _CurrentStateE;
}

void UFSMComponent::SetPreviousPatternState(EBossState State)
{
	if (!IsReactionState(State) && State != EBossState::NORMALATTACK && CurrentStateE != EBossState::STRAFE)
		PrevStateE = CurrentStateE;
}