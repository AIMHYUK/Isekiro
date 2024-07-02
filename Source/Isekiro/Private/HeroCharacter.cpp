// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "HeroCharacter.h"
#include "Components/InputComponent.h"
#include "Components/Boxcomponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h" //메테리얼 인스턴스 사용
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" //향상된 입력
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "HeroAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h" //콜리전채널추가
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "RealParryBox.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AHeroCharacter::AHeroCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm); //생성자에서 만들자..

	MaxCombo = 3;
	AttackEndComboState();

	//초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	//초기 상태를 대시 가능으로 설정
	bCanLaunch = true;
	//기본마찰력상태 변수 초기화
	OriginalGroundFriction = GetCharacterMovement()->GroundFriction;

	//공격,방어 체크박스
	ParryCheck = CreateDefaultSubobject<URealParryBox>(TEXT("ParryCheck"));
	ParryCheck->SetupAttachment(RootComponent);


}

void AHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(Context, 0);
		}
	}
	// Ensure the owner is valid and has a mesh component
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	ParryCheck->AttachToComponent(CharacterMesh,FAttachmentTransformRules::KeepRelativeTransform, TEXT("ParryCheckBox"));

	if (GetCapsuleComponent() != nullptr)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHeroCharacter::PlayHittedMontage);
		UE_LOG(LogTemp, Error, TEXT("Clear"));
	}
}

void AHeroCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HeroAnim = Cast<UHeroAnimInstance>(GetMesh()->GetAnimInstance());

	HeroAnim->OnMontageEnded.AddDynamic(this, &AHeroCharacter::OnAttackMontageEnded);

	HeroAnim->OnNextAttackCheck.AddLambda([this]() -> void
		{
			CanNextCombo = false;

			if (IsComboInputOn)
			{
				AttackStartComboState();
				HeroAnim->JumpToAttackMontageSection(CurrentCombo);
			}
		});
	HeroAnim->OnAttackHitCheck.AddLambda([this]() -> void
		{
			
			DealDamage();
			//OnWidget();
			UE_LOG(LogTemp, Error, TEXT("ATC"));
		});	
}
// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked< UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Jump);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Guard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &AHeroCharacter::Guard);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AHeroCharacter::AttackStart);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Run);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Dash);
	}
	
}
void AHeroCharacter::Move(const FInputActionValue& value)
{
	if (!IsAttacking)
	{
		const FVector2D Vector = value.Get<FVector2D>();
		FRotator Rotation = GetController()->GetControlRotation(); //플레이어의 방향 읽어서 
		FRotator YawRotation(0, Rotation.Yaw, 0); //yaw사용

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, Vector.Y);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, Vector.X);
	}
}
void AHeroCharacter::Look(const FInputActionValue& value) 
{
	const FVector2D LookAxisValue = value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerPitchInput(LookAxisValue.Y);
		AddControllerYawInput(LookAxisValue.X);
	}

}

void AHeroCharacter::Jump(const FInputActionValue& value)
{
	if (!IsAttacking)
	{
		Super::ACharacter::Jump();
		GetCharacterMovement()->JumpZVelocity = 800.0f;//800으로 임의 상승
		GetCharacterMovement()->GravityScale = 2.0f; //중력 2배
		/*UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Hello", true, true, FLinearColor(0.0f, 0.66f, 1.0f, 1.0f), 2.0f);*/
	}
}
void AHeroCharacter::Guard(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	ParryCheck->ParryStarted();
	bool bIsParrying = ParryCheck->GetParryWindow();
	GuardState = ECharacterGuardState::ECGS_Guarding;
	// Get the animation instance
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// Check if the button is pressed
		if (bIsPressed)
		{
			// Play the guard montage
			AnimInstance->Montage_Play(KeepGuardMontage);
		}

		else if(bIsParrying)
		{
					
			AnimInstance->Montage_Play(GuardMontage);	
		}
	}

}

void AHeroCharacter::PlayParryMontage()
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetIgnoreMoveInput(true); //패링중 못움직이게
	
	ParryMontageSections = { TEXT("Parry1"), TEXT("Parry2"), TEXT("Parry3") }; //섹션 이름 받아서

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ParryMontage);
		if (ParryMontageSections.Num() > 0)
		{
			// Select a random section from the array
			int32 SectionIndex = FMath::RandRange(0, ParryMontageSections.Num() - 1);
			FName SelectedSection = ParryMontageSections[SectionIndex]; //랜덤하게 플레이하기

			// Jump to the selected section
			AnimInstance->Montage_JumpToSection(SelectedSection, ParryMontage);
			// Log the selected section (optional, for debugging)
			UE_LOG(LogTemp, Log, TEXT("Playing Parry Montage Section: %s"), *SelectedSection.ToString());
		}
		KnockBack();

		float MontageDuration = ParryMontage->GetPlayLength(); // 몽타주의 길이
		GetWorld()->GetTimerManager().SetTimer(KnockBackTimerHandle, this, &AHeroCharacter::KnockBack, 0.1f, true);
		
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &AHeroCharacter::OnParryMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ParryMontage);
}

void AHeroCharacter::OnParryMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 타이머를 멈추고 플레이어 입력을 재활성화
	GetWorld()->GetTimerManager().ClearTimer(KnockBackTimerHandle);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(false); // 패링 및 넉백 후 움직임 재활성화
	}
}

void AHeroCharacter::KnockBack() // 뒤로 밀리는 함수
{
	GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
	FVector KnockBackDirection = -GetActorForwardVector();
	float KnockBackDistance = 50.0f; // 지속적으로 밀리는 거리

	LaunchCharacter(KnockBackDirection * KnockBackDistance, true, true);
}
void AHeroCharacter::ShakeCam()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CamShake); // 여기에 사용할 CameraShake 클래스를 설정하세요
		CameraManager->PlayWorldCameraShake(GetWorld(), CamShake, GetActorLocation(), 0, 500, 1);
	}
}

void AHeroCharacter::PlayHittedMontage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetIgnoreMoveInput(true);
	UE_LOG(LogTemp, Error, TEXT("HIt!"));
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(HittedMontage);
	}
	PlayerController->SetIgnoreMoveInput(false);

}

void AHeroCharacter::Run(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Display, TEXT("Run"));
	auto movement = GetCharacterMovement();
	//현재 달리기 모드라면
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		//걷기속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}
	//현재 달리기 모드가 아니라면
	else
	{
		//달리기속도로 전환
		movement->MaxWalkSpeed = runSpeed;
	}
}

void AHeroCharacter::AttackStart(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Display, TEXT("CurrentCombo = %d"), CurrentCombo);
	if (IsAttacking)
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		DealDamage();
		AttackStartComboState();
		HeroAnim->PlayAttackMontage();
		HeroAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHeroCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	AttackEndComboState();
	//UE_LOG(LogTemp, Error, TEXT("onEnded, Currentcombo = %d"), CurrentCombo);
}

void AHeroCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHeroCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AHeroCharacter::DealDamage()
{
	if (ParryCheck)
	{
		FVector SphereLocation = ParryCheck->GetComponentLocation();
		float SphereSize = ParryCheck->GetScaledSphereRadius();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1)); // 예시로 LockOnPawn 타입 추가

		TArray<AActor*> ActorsToIgnore;
		TArray<AActor*> OutActors;

		UKismetSystemLibrary::SphereOverlapActors(
			GetWorld(),
			SphereLocation,
			SphereSize,
			ObjectTypes,
			nullptr, // QueryParams, 기본값 사용 (모든 타입의 액터와 오버랩)
			ActorsToIgnore,
			OutActors
		);

		for (AActor* OverlappedActor : OutActors)
		{
			FVector ActorLocation = OverlappedActor->GetActorLocation();
			float SphereRadius = 50.0f; // 구의 반지름 설정

			FLinearColor LineColor(0, 1, 0, 1); // 라인 색상 설정 (초록색)

			// DrawDebugSphere 함수를 사용하여 구체 그리기
			UKismetSystemLibrary::DrawDebugSphere(
				GetWorld(),
				ActorLocation,
				SphereRadius,
				12, // Segments (구체 표현을 위한 세그먼트 수)
				LineColor,
				true, // PersistentLines (라이프타임 동안 지속됨)
				5.0f // Duration (지속 시간)
			);
		}
	}
}
	

//대쉬 이벤트처리 함수 구현
void AHeroCharacter::Dash(const FInputActionValue& value)
{
	//만약 지금 대쉬가 가능한 상태라면
	if (bCanLaunch)
	{
		//1. cooldown이 끝날때까지 조건을 false로 변경
		bCanLaunch = false;
		//2. 대쉬하는 동안 마찰력 0인 상태로 변경
		GetCharacterMovement()->GroundFriction = 0.0f;
		//3. 앞으로 이동하고
		LaunchFoward();

		////4. 0.01 딜레이 후 UpVector 이동
		//GetWorldTimerManager().SetTimer(LaunchUpwardTimeHandle, this, &AHeroCharacter::LaunchUpward, 0.01f, false);
		// 
		//5. 1초 딜레이 후 Cooldown 초기화
		GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AHeroCharacter::ResetLaunchCooldown, 0.5f, false);
		//6. 쿨다운 초기화 후 마찰력 복구
		GetWorldTimerManager().SetTimer(ResetFrictionTimerHandle, this, &AHeroCharacter::ResetGroundFriction, 1.0f, false);
	}
}

//앞으로 대쉬 기능 함수 구현
void AHeroCharacter::LaunchFoward()
{
	//앞으로 이동시킬 Velocity
	FVector LaunchVelocity = GetVelocity().GetSafeNormal() * 750;
	//캐릭터를 앞으로 이동
	LaunchCharacter(LaunchVelocity, true, false);
}
//Cooldown초기화 함수 구현
void AHeroCharacter::ResetLaunchCooldown()
{
	bCanLaunch = true;
}

//마찰력상태 초기화 함수 구현
void AHeroCharacter::ResetGroundFriction()
{
	GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
}

