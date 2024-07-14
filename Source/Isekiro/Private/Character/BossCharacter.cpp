// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"
#include "FSM/FSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FSM/StateObject.h"
#include "Components/BoxComponent.h"
#include "Arrow.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "ActorComponents/StatusComponent.h"
#include "BossWidget.h"
#include "IsekiroGameModeBase.h"
#include "Components/WidgetComponent.h"

ABossCharacter::ABossCharacter()
{
	FSMComponent = CreateDefaultSubobject<UFSMComponent>("FSMComponent");

	AttackBoxComp = CreateDefaultSubobject<UBoxComponent>("AttackBoxComponent");
	AttackBoxComp->SetupAttachment(RootComponent);
	AttackBoxComp->SetRelativeLocation(FVector(110.f, 0.f, 0.f));
	AttackBoxComp->SetBoxExtent(FVector(100.f, 80.f, 75.f));
	AttackBoxComp->SetCollisionProfileName("Arrow");

	TargetOffset = 80.0f;
	TargetOffsetBuffer = 5.f;
	NearSpaceBuffer = 200.f;

	DefaultSetting.Damage = 20.f;
	DefaultSetting.Speed = 1800.f;
	HardSetting.Damage = 30.f;
	HardSetting.Speed = 2600.f;

	CurrDir = EDirection::BACK;

	bLockOnTarget = true;

	LockOnComponent = CreateDefaultSubobject<UCapsuleComponent>("LockOnComponent");
	LockOnComponent->SetupAttachment(RootComponent);
	LockOnComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	LockOnComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	LockOnComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LockOnComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	Tags.Add(FName("LockOnPossible"));

	TargetWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("TargetWidgetComponent");
	TargetWidgetComponent->SetupAttachment(GetMesh());
	TargetWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto Widget = TargetWidgetComponent->GetWidget();
	if (Widget)
	{
		Widget->SetVisibility(ESlateVisibility::Hidden);
	}

	HeightZ = GetActorLocation().Z;

	if (AttackBoxComp)
	{
		AttackBoxComp->IgnoreActorWhenMoving(this, true);

		AttackBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnAttackBoxOverlapped);

		AttackBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (StatusComponent)
	{
		StatusComponent->OnStatusChanged.AddDynamic(this, &ABossCharacter::OnStatusChanged);
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnCapsuleOverlapped);


	auto GM = GetWorld()->GetAuthGameMode<AIsekiroGameModeBase>();
	if (GM && GM->BossUI)
	{
		BossUI = GM->BossUI;
	}
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensureAlways(Target) && bLockOnTarget)
	{
		FVector DirVector = Target->GetActorLocation() - GetActorLocation();
		DirVector.Normalize();
		FRotator newRotation = DirVector.Rotation();
		newRotation.Pitch = 0.f;
		newRotation.Roll = 0.f;
		SetActorRotation(newRotation.Quaternion());
	}
	if (CanRecoverPosture())
	{
		StatusComponent->RecoverPosture(3.f);
	}
	/*UE_LOG(LogTemp, Warning, TEXT("POsture: %f"), StatusComponent->GetPosture());*/
}

bool ABossCharacter::CanRecoverPosture() const
{
	if (StatusComponent && FSMComponent)
	{
		EBossState CurrState = FSMComponent->GetCurrentStateE();
		return FSMComponent->CanStun() && CurrState != EBossState::DEATH && CurrState != EBossState::PARRY && CurrState != EBossState::HIT;
	}

	return false;
}

UBossWidget* ABossCharacter::GetBossUI() const
{
	return BossUI;
}

void ABossCharacter::BeginAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Attack"));
	AttackBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossCharacter::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("End Attack"));
	AttackBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacter::FireArrow()
{
	SetupFireArrow(DefaultSetting);
}

void ABossCharacter::FireArrowHard()
{
	SetupFireArrow(HardSetting);
}

AActor* ABossCharacter::GetTarget() const
{
	return Target;
}

void ABossCharacter::OnAttackBoxOverlapped(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping with %s"), *OtherActor->GetName());
}

void ABossCharacter::OnCapsuleOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if()
}

void ABossCharacter::OnStatusChanged(float OldHealth, float OldPosture, float NewHealth, float NewPosture)
{
	if (StatusComponent && FSMComponent)
	{
		if (StatusComponent->IsPostureBroken() || !StatusComponent->HasHealth() || !StatusComponent->IsAlive())
		{
			if (FSMComponent && FSMComponent->GetCurrentStateE() != EBossState::DEATH)
			{
				FSMComponent->ChangeStateTo(EBossState::DEATH);
			}
		}
		else
		{
			if (FSMComponent->CanStun())
			{
				if (FSMComponent->CanDefend())
				{
					StatusComponent->SetHealth(OldHealth);
					FSMComponent->StartParryOrBlock();
				}
				else FSMComponent->ChangeStateTo(EBossState::HIT);
			}
		}
	}
}

void ABossCharacter::SetupFireArrow(FArrowSetting Setting)
{
	if (Target && ArrowClass)
	{
		FVector DirVector = Target->GetActorLocation() - GetActorLocation();
		FRotator Rotate = DirVector.Rotation();
		FTransform Trans;

		Trans.SetLocation(GetActorLocation() + GetActorForwardVector() * 10.f);
		Trans.SetRotation(Rotate.Quaternion());
		Trans.SetScale3D(FVector::One());

		AArrow* Spawned = GetWorld()->SpawnActorDeferred<AArrow>(ArrowClass, Trans, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Spawned)
		{
			Spawned->Initialize(Target, Setting);
		}
		UGameplayStatics::FinishSpawningActor(Spawned, Trans);
	}
}

float ABossCharacter::GetDistanceToTargetOffset() const
{
	if (Target)
	{
		FVector BossLoc = GetActorLocation();
		BossLoc.Z = HeightZ;
		FVector DirVector = GetTargetOffsetLocation() - BossLoc;
		return FMath::Sqrt(DirVector.Dot(DirVector));
	}

	return -1.f;
}

FVector ABossCharacter::GetDirectionVectorToTarget() const
{
	FVector DirVector = GetTargetOffsetLocation() - GetActorLocation();
	DirVector.Normalize();
	return DirVector;
}

FVector ABossCharacter::GetNewMovementLocation(float DistanceToTravel) const
{
	FVector DirVec = GetDirectionVectorToTarget();
	DirVec *= DistanceToTravel;
	FVector NewLoc = DirVec + GetActorLocation();

	float NewDist = FVector::DistSquared(NewLoc, GetActorLocation());
	float TargetDist = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());

	if (NewDist < TargetDist) // if new boss location is within the Target
	{
		if (IsWithinTarget(NewLoc, TargetOffsetBuffer))
			return GetTargetOffsetLocation();
		else return NewLoc;
	}
	else // if new boss location is beyond the Target
	{
		return GetTargetOffsetLocation();
	}
}

FVector ABossCharacter::GetTargetOffsetLocation() const
{
	if (Target)
	{
		FVector End;
		End = IsLockedOnTarget() ? End = Target->GetActorLocation() : /*Set End as infront of boss*/ End = FVector::Zero();
		FVector Start = GetActorLocation();
		End.Z = HeightZ;
		Start.Z = HeightZ;

		FVector TargetVector = End - Start;
		FVector Dir = TargetVector;
		Dir.Normalize();
		Dir *= TargetOffset;
		TargetVector -= Dir;

		return TargetVector + GetActorLocation();
	}
	return FVector();
}

FVector ABossCharacter::GetTargetLoc() const
{
	FVector Loc = Target->GetActorLocation();
	Loc.Z = HeightZ;
	return Loc;
}

float ABossCharacter::GetTargetOffset() const
{
	return TargetOffset;
}

bool ABossCharacter::IsLockedOnTarget() const
{
	return true;
}

void ABossCharacter::SetLockOnTarget(bool _bLockOnTarget)
{
	bLockOnTarget = _bLockOnTarget;
}

EDirection ABossCharacter::GetCurrentDirection() const
{
	return CurrDir;
}

bool ABossCharacter::IsWithinNearRange() const
{
	return IsWithinTarget(GetActorLocation(), NearSpaceBuffer);
}

bool ABossCharacter::IsWithinTarget() const
{
	return IsWithinTarget(GetActorLocation(), TargetOffsetBuffer);
}

bool ABossCharacter::IsWithinTarget(FVector Location, float Offset) const
{
	if (Target)
	{
		FVector TargetTemp = Target->GetActorLocation();
		TargetTemp.Z = HeightZ;

		FVector OffsetDir = TargetTemp - GetTargetOffsetLocation();
		OffsetDir.Normalize();
		OffsetDir *= -1.f;
		FVector BufferLoc = OffsetDir * Offset + GetTargetOffsetLocation(); // if boss is within this distance, EFightingSpace is NEAR. Else EFightingSpace is FAR.
		FVector BufferDistVector = TargetTemp - BufferLoc;

		FVector LocDistVector = TargetTemp - Location;
		FVector BossTemp = GetActorLocation();
		BossTemp.Z = HeightZ;
		FVector BossVector = TargetTemp - BossTemp;
		float dotResult = FVector::DotProduct(LocDistVector, BossVector);
		if (dotResult < 0.f)
		{
			LocDistVector *= -1.f;
		}

		float BufferDistSq = FVector::DotProduct(BufferDistVector, BufferDistVector);
		float LocDistSq = FVector::DotProduct(LocDistVector, LocDistVector);

		return LocDistSq < BufferDistSq;
	}
	return false;
}
