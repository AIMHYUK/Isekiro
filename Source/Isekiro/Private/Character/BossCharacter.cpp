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
#include "HeroCharacter.h"

ABossCharacter::ABossCharacter()
{
	FSMComponent = CreateDefaultSubobject<UFSMComponent>("FSMComponent");

	SetActorRelativeScale3D(FVector(1.25f, 1.25f, 1.25f));

	AttackBoxComp = CreateDefaultSubobject<UBoxComponent>("AttackBoxComponent");
	AttackBoxComp->SetupAttachment(RootComponent);
	AttackBoxComp->SetRelativeLocation(FVector(80.f, 0.f, 0.f));
	AttackBoxComp->SetBoxExtent(FVector(80.f, 24.f, 24.f));
	AttackBoxComp->SetCollisionProfileName("Arrow");

	PerilousSweepBoxComp = CreateDefaultSubobject<UBoxComponent>("PerilousSweepBoxCompnent");
	PerilousSweepBoxComp->SetupAttachment(RootComponent);
	PerilousSweepBoxComp->SetRelativeLocation(FVector(80.f, 0.f, -30.f));
	PerilousSweepBoxComp->SetBoxExtent(FVector(80.f, 24.f, 24.f));
	PerilousSweepBoxComp->SetCollisionProfileName("Arrow");

	TargetOffset = 120.0f;
	TargetOffsetBuffer = 30.f;
	NearSpaceBuffer = 300.f;

	DefaultSetting.Damage = 20.f;
	DefaultSetting.Speed = 5600.f;
	HardSetting.Damage = 30.f;
	HardSetting.Speed = 5600.f;

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
	GetMesh()->SetVisibility(false);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	RetargetedSKMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RetargetedSkeletalMeshComponent");
	RetargetedSKMesh->SetupAttachment(GetMesh());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Tags.Add(FName("LockOnPossible"));

	TargetWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("TargetWidgetComponent");
	TargetWidgetComponent->SetupAttachment(GetMesh(), "Spine2");
	TargetWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	BowEquippedMesh = CreateDefaultSubobject<UStaticMeshComponent>("BowEquippedMesh");
	BowEquippedMesh->SetupAttachment(RetargetedSKMesh, "BowEquippedSocket");
	BowEquippedMesh->SetRelativeScale3D(FVector(1.6f));
	BowEquippedMesh->SetCollisionProfileName("NoCollision");

	BowStashedMesh = CreateDefaultSubobject<UStaticMeshComponent>("BowStashedMesh");
	BowStashedMesh->SetupAttachment(RetargetedSKMesh, "BowSheathedSocket");
	BowStashedMesh->SetRelativeScale3D(FVector(1.6f));
	BowStashedMesh->SetCollisionProfileName("NoCollision");

	KatanaEquippedMesh = CreateDefaultSubobject<UStaticMeshComponent>("KatanaEquippedMesh");
	KatanaEquippedMesh->SetupAttachment(RetargetedSKMesh, "KatanaEquippedSocket");
	KatanaEquippedMesh->SetRelativeScale3D(FVector(1.25f));
	KatanaEquippedMesh->SetCollisionProfileName("NoCollision");

	KatanaStashedMesh = CreateDefaultSubobject<UStaticMeshComponent>("KatanaStashedMesh");
	KatanaStashedMesh->SetupAttachment(RetargetedSKMesh, "KatanaSheathSocket");
	KatanaStashedMesh->SetRelativeScale3D(FVector(1.25f));
	KatanaStashedMesh->SetCollisionProfileName("NoCollision");

	ScabbardMesh = CreateDefaultSubobject<UStaticMeshComponent>("ScabbardMesh");
	ScabbardMesh->SetupAttachment(RetargetedSKMesh, "ScabbardSocket");
	ScabbardMesh->SetRelativeScale3D(FVector(1.25f));
	ScabbardMesh->SetCollisionProfileName("NoCollision");
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("MeshName: %s"), *GetNameSafe(RetargetedSKMesh));

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

	if (PerilousSweepBoxComp)
	{
		PerilousSweepBoxComp->IgnoreActorWhenMoving(this, true);

		PerilousSweepBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnPerilousSweepBoxOverlapped);

		PerilousSweepBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	EquipKatana();

	if (!Target)
	{
		auto Hero = UGameplayStatics::GetActorOfClass(GetWorld(), AHeroCharacter::StaticClass());
		if (Hero)
		{
			Target = Hero;
		}
	}

}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("CompVel: %s"), *GetCapsuleComponent()->GetComponentVelocity().ToString()));

	if (Target && bLockOnTarget)
	{
		auto Status = Target->GetComponentByClass<UStatusComponent>();
		if (Status && Status->HasHealth())
		{
			FVector DirVector = Target->GetActorLocation() - GetActorLocation();
			DirVector.Normalize();
			FRotator newRotation = DirVector.Rotation();
			newRotation.Pitch = 0.f;
			newRotation.Roll = 0.f;
			SetActorRotation(newRotation.Quaternion());
		}
	}

	if (StatusComponent && !StatusComponent->IsAlive())
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	if (CanRecoverPosture())
	{
		StatusComponent->RecoverPosture(3.f);
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Emerald, FString::Printf(TEXT("Distance: %f"), GetDistanceToTargetOffset()));
}

bool ABossCharacter::CanRecoverPosture() const
{
	if (StatusComponent && FSMComponent)
	{
		return FSMComponent->CanRecoverPosture();
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

void ABossCharacter::BeginPerilousAttack(EPerilousAttackType Type)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Perilous Attack"));

	switch (Type)
	{
	case EPerilousAttackType::SWEEP:
		PerilousSweepBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;
	case EPerilousAttackType::THRUST:
		AttackBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;
	}
}

void ABossCharacter::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("End Attack"));
	AttackBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PerilousSweepBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ABossCharacter::OnPerilousSweepBoxOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping with %s"), *OtherActor->GetName());
	PerilousSweepBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacter::OnAttackBoxOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping with %s"), *OtherActor->GetName());
	AttackBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacter::OnCapsuleOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if()
}

void ABossCharacter::OnStatusChanged(AActor* Initiator, float OldHealth, float OldPosture, float NewHealth, float NewPosture)
{
	if (!ensure(Initiator)) return;
	UE_LOG(LogTemp, Warning, TEXT("AAAAAAAAAAAAAAAAAAAAAAAAA"));
	if (StatusComponent && FSMComponent)
	{
		if (FSMComponent->GetCurrentStateE() == EBossState::DODGE)
		{
			StatusComponent->SetHealth(OldHealth);
			StatusComponent->SetPosture(OldPosture);
			return;
		}

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

					auto Hero = Cast<AHeroCharacter>(Initiator);
					if (Hero && Hero->IsParrying()) 
						FSMComponent->ChangeStateTo(EBossState::DEFLECTED);
					else FSMComponent->StartParryOrBlock();
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

		Trans.SetLocation(GetActorLocation());
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

float ABossCharacter::GetDistanceToTarget() const
{
	if (Target)
	{
		FVector BossLoc = GetActorLocation();
		BossLoc.Z = HeightZ;


		FVector TargetLoc = Target->GetActorLocation();
		TargetLoc.Z = HeightZ;

		FVector DirVector = TargetLoc - BossLoc;
		return FMath::Sqrt(DirVector.Dot(DirVector));
	}

	return -1.f;
}

FVector ABossCharacter::GetDirectionVectorToTarget() const
{
	FVector Loc = GetActorLocation();
	Loc.Z = HeightZ;
	FVector DirVector = GetTargetLoc() - Loc;
	DirVector.Normalize();
	return DirVector;
}

FVector ABossCharacter::GetDirectionVector(EDirection Dir) const
{
	switch (Dir)
	{
	case EDirection::FORWARD:
		return GetDirectionVectorToTarget();
	case EDirection::BACK:
		return GetDirectionVectorToTarget() * -1.f;
	case EDirection::RIGHT:
		return GetActorRightVector();
	case EDirection::LEFT:
		return GetActorRightVector() * -1.f;
	}
	return FVector();
}

FVector ABossCharacter::GetNewMovementLocation(float DistanceToTravel, EDirection Dir) const
{
	FVector DirVec = GetDirectionVector(Dir);
	DirVec *= DistanceToTravel;
	FVector NewLoc = DirVec + GetActorLocation();

	if (Dir == EDirection::FORWARD)
	{
		float NewDist = FVector::DistSquared(NewLoc, GetActorLocation());
		float TargetDist = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());
		if (NewDist < TargetDist) // if new boss location is within the Target
		{
			if (IsWithinTarget(NewLoc, TargetOffsetBuffer))
				return GetTargetOffsetLocation();
		}
		else // if new boss location is beyond the Target
		{
			return GetTargetOffsetLocation();
		}
	}

	return NewLoc;
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

void ABossCharacter::ResetHeight()
{
	FVector Loc = GetActorLocation();
	Loc.Z = HeightZ;
	SetActorLocation(Loc);
}

void ABossCharacter::EquipKatana()
{
	BowEquippedMesh->SetVisibility(false);
	BowStashedMesh->SetVisibility(true);

	KatanaEquippedMesh->SetVisibility(true);
	KatanaStashedMesh->SetVisibility(false);
}

void ABossCharacter::EquipBow()
{
	BowEquippedMesh->SetVisibility(true);
	BowStashedMesh->SetVisibility(false);

	KatanaEquippedMesh->SetVisibility(false);
	KatanaStashedMesh->SetVisibility(true);
}

void ABossCharacter::AdjustKatana()
{
	KatanaEquippedMesh->AttachToComponent(GetMesh(), 
		FAttachmentTransformRules::SnapToTargetIncludingScale, "RightHandSocketSheath");
}

void ABossCharacter::AdjustKatana1()
{
	KatanaEquippedMesh->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale, "RightHandSocketSheath1");
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
