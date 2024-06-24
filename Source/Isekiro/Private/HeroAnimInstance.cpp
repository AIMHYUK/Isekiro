// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroAnimInstance.h"
#include "HeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UHeroAnimInstance::UHeroAnimInstance()
{
	static ConstructorHelpers::FObjectFinder <UAnimMontage> ATTACK_MONTAGE
	(TEXT("/Script/Engine.AnimMontage'/Game/AM_Attack.AM_Attack'"));

	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}
void UHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	HeroCharacter = Cast<AHeroCharacter>(TryGetPawnOwner());
	if (HeroCharacter)
	{
        HeroCharacterMovement = HeroCharacter->GetCharacterMovement();
	}
}

void UHeroAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if (HeroCharacterMovement)
    {
        GroundSpeed = UKismetMathLibrary::VSizeXY(HeroCharacterMovement->Velocity);
        bIsFalling = HeroCharacterMovement->IsFalling();
    }
}

void UHeroAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.0f);
}

void UHeroAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UHeroAnimInstance::AnimNotify_AttackHitCheck()
{
	//Broadcast = 델리게이트 안에 있는 모든 함수 실행
	OnAttackHitCheck.Broadcast();
}

void UHeroAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UHeroAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}