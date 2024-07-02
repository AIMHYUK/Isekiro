// Fill out your copyright notice in the Description page of Project Settings.


#include "RealParryBox.h"
#include "ParryBox.h"
#include "HeroAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "HeroCharacter.h"

// Sets default values for this component's properties
URealParryBox::URealParryBox()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InitSphereRadius(50.0f); // Example: Set the sphere radius
	// Set collision settings
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap); // Example: Respond to GameTraceChannel1



}

// Called when the game starts
void URealParryBox::BeginPlay()
{
	Super::BeginPlay();

	// ParryCheck에 대한 충돌 이벤트 핸들러를 바인딩합니다.

	OnComponentBeginOverlap.AddDynamic(this, &URealParryBox::OnParryCheckBeginOverlap);
}

void URealParryBox::OnParryCheckBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MyCharacter = Cast<AHeroCharacter>(GetOwner());
	if (MyCharacter)
	{
		MyCharacter->PlayParryMontage();
		UGameplayStatics::SetGlobalTimeDilation(this, 0.2f);

		// Set a timer to reset time dilation after a short duration
		GetWorld()->GetTimerManager().SetTimer(TimeDilationHandle, this, &URealParryBox::ResetTimeDilation, 0.5f, false);
	}
	else
		UE_LOG(LogTemp, Log, TEXT("Failed"));
}


// Called every frame
void URealParryBox::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URealParryBox::ParryStarted()
{
	bIsParryWindow = true;
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetVisibility(true);

	// Set collision response specifically for ECC_GameTraceChannel2
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);

	// Log collision and visibility status
	FString CollisionStatus = (GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics) ? TEXT("Enabled") : TEXT("Disabled");
	FString VisibilityStatus = IsVisible() ? TEXT("Visible") : TEXT("Hidden");
	UE_LOG(LogTemp, Warning, TEXT("Parry Window Started: Collision %s, Visibility %s"), *CollisionStatus, *VisibilityStatus);

	// Set a timer to end the parry window after 0.2 seconds
	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &URealParryBox::ParryEnded, 0.2f, false);
}

void URealParryBox::ParryEnded()
{
	bIsParryWindow = false;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetVisibility(false);

	// Log collision and visibility status
	FString CollisionStatus = (GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics) ? TEXT("Enabled") : TEXT("Disabled");
	FString VisibilityStatus = IsVisible() ? TEXT("Visible") : TEXT("Hidden");
	UE_LOG(LogTemp, Warning, TEXT("Parry Window Ended: Collision %s, Visibility %s"), *CollisionStatus, *VisibilityStatus);

	// Additional logic to handle the end of parry window
}
bool URealParryBox::GetParryWindow()
{
	return bIsParryWindow;
}
void URealParryBox::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}