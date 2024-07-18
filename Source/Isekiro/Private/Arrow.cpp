// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ActorComponents/StatusComponent.h"
#include "HeroCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AArrow::AArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComp->SetupAttachment(SceneComp);
	SphereComp->SetSphereRadius(32.f);
	SphereComp->SetRelativeLocation(FVector(70.f, 0.f, 0.f));
	SphereComp->SetCollisionProfileName("Arrow");

	SMComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SMComp->SetupAttachment(SceneComp);
	FRotator Rotate;
	Rotate.Yaw = -90.f;
	SMComp->SetRelativeRotation(Rotate.Quaternion());
	SMComp->SetCollisionProfileName("NoCollision");

	ArrowTrailComp = CreateDefaultSubobject<UNiagaraComponent>("ArrowTrailComponent");
	ArrowTrailComp->SetupAttachment(SceneComp);

	bHasHit = false;
}

void AArrow::Initialize(AActor* _Target, FArrowSetting _ArrowSetting)
{
	if (_Target)
	{
		Target = _Target;
	}
	ArrowSetting = _ArrowSetting;
}

void AArrow::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->IgnoreActorWhenMoving(Owner, true);
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();

	ArrowTrailComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ArrowTrail, SMComp, "",
		FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

	if (Target)
	{
		Dir = Target->GetActorLocation() - GetActorLocation();
		Dir.Normalize();
	}
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlapBegin);
	SetLifeSpan(2.f);
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasHit)
	{
		FVector DirVector = Dir * ArrowSetting.Speed * DeltaTime;
		SetActorLocation(DirVector + GetActorLocation());
	}
}

void AArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (SMComp)
		SMComp->SetVisibility(false);
	
	bHasHit = true;
}

