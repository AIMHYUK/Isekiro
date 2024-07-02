// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

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
	if (Target)
	{
		Dir = Target->GetActorLocation() - GetActorLocation();
		Dir.Normalize();
	}
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlapBegin);
	SetLifeSpan(5.f);
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DirVector = Dir * ArrowSetting.Speed * DeltaTime;
	SetActorLocation(DirVector + GetActorLocation());
}

void AArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Black, FString::Printf(TEXT("Arrow hit %s"), *GetNameSafe(OtherActor)));
	}
}

