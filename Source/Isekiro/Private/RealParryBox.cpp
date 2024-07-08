// Fill out your copyright notice in the Description page of Project Settings.


#include "RealParryBox.h"
#include "ParryBox.h"
#include "HeroAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "IsekiroGameModeBase.h"
#include "ActorComponents/StatusComponent.h"
#include "HeroCharacter.h"

// Sets default values for this component's properties
URealParryBox::URealParryBox()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InitSphereRadius(30.0f); // Example: Set the sphere radius
	// Set collision settings
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap); // Example: Respond to GameTraceChannel1

	bCanParry = true;
	/*State = Cast<UStatusComponent>(GetWorld()->GetAuthGameMode());*/
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
	if (MyCharacter && bIsParryWindow)
	{
		MyCharacter->PlayParryMontage();
		UGameplayStatics::SetGlobalTimeDilation(this, 0.8f);
		UStatusComponent* State = MyCharacter->GetStatusComponent();
		State->ApplyPostureDamage(5);
		MyCharacter->KnockBack(500);
		// Set a timer to reset time dilation after a short duration
		GetWorld()->GetTimerManager().SetTimer(TimeDilationHandle, this, &URealParryBox::ResetTimeDilation, 0.5f, false);
	}
}


// Called every frame
void URealParryBox::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URealParryBox::ParryStarted()
{

	if (!bCanParry)
	{
		return; //패링 쿨다운 중이면 패링시작하지 않음

	} 
	bIsParryWindow = true; //패리 상태
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	MyCharacter = Cast<AHeroCharacter>(GetOwner());
	MyCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore); //패링상태이면 데미지 안받음
	if (!MyCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) //패리상태로 가드 올리기
	{
		MyCharacter->PlayGuardMontage(); //가드 몽타주 실행 -> 후에 가드히트로 바꿔야함.
	}
		
	UE_LOG(LogTemp, Display, TEXT("GuardMontage"));
	// ECC_GameTraceChannel3에 대한 충돌 응답 설정
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);

	// 패링 종료 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &URealParryBox::ParryEnded, 8.0f / 60.0f, false);

	// 패링 쿨다운 설정
	bCanParry = false;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &URealParryBox::ResetParryCooldown, 8.0f / 60.0f, false);
}

void URealParryBox::ParryEnded()
{
	bIsParryWindow = false;
	
	MyCharacter = Cast<AHeroCharacter>(GetOwner());
	MyCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
	
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool URealParryBox::GetParryWindow()
{
	return bIsParryWindow;
}
void URealParryBox::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}

void URealParryBox::ResetParryCooldown()
{
	bCanParry = true;
}
