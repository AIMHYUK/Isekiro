// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"
#include "FSM/FSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FSM/StateObject.h"
#include "Components/BoxComponent.h"
#include "Arrow.h"
#include "Kismet/GameplayStatics.h"

ABossCharacter::ABossCharacter()
{
	FSMComponent = CreateDefaultSubobject<UFSMComponent>("FSMComponent");

	AttackBoxComp = CreateDefaultSubobject<UBoxComponent>("AttackBoxComponent");
	AttackBoxComp->SetupAttachment(RootComponent);
	AttackBoxComp->SetRelativeLocation(FVector(100.f, 0.f, 0.f));
	AttackBoxComp->SetBoxExtent(FVector(75.f, 50.f, 75.f));

	TargetOffset = 200.f;

	DefaultSetting.Damage = 20.f;
	DefaultSetting.Speed = 1800.f;
	HardSetting.Damage = 30.f;
	HardSetting.Speed= 2600.f;

	CurrDir = EDirection::LEFT;
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	HeightZ = GetActorLocation().Z;

	if (AttackBoxComp)
	{
		AttackBoxComp->IgnoreActorWhenMoving(this, true);

		AttackBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnAttackBoxOverlapped);
	}
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensureAlways(Target))
	{
		FVector DirVector = Target->GetActorLocation() - GetActorLocation();
		DirVector.Normalize();
		FRotator newRotation = DirVector.Rotation();
		newRotation.Pitch = 0.f;
		newRotation.Roll = 0.f;
		SetActorRotation(newRotation.Quaternion());
	}
}

void ABossCharacter::BeginAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Attack"));

	FHitResult Hit;
	FVector Start = AttackBoxComp->GetComponentLocation();
	FVector End = AttackBoxComp->GetComponentLocation();

	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FCollisionShape Shape;
	float Radius = AttackBoxComp->GetScaledBoxExtent().Z;
	Shape.SetSphere(Radius);

	DrawDebugSphere(GetWorld(), Start, Radius, 32, FColor::Black, true);

	if (GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjQueryParam, Shape, QueryParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("what did we hit? %s"), *Hit.GetActor()->GetName());
	}
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

float ABossCharacter::GetDistanceToTarget() const
{
	if (Target)
	{
		FVector DirVector = Target->GetActorLocation() - GetActorLocation();
		return FMath::Sqrt(DirVector.Dot(DirVector));
	}

	return -1.f;
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

bool ABossCharacter::IsLockedOnTarget() const
{
	return true;
}

EDirection ABossCharacter::GetCurrentDirection() const
{
	return CurrDir;
}

void ABossCharacter::StartParry()
{
	FSMComponent->ChangeStateTo(EBossState::PARRY);
}
