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

	//공격,방어 체크박스
	// BoxComponent 생성
	AttackGuardCheckBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackGuardCheckBox"));

	// 캐릭터 메쉬의 특정 소켓에 컴포넌트를 붙임 (Weapon_r 소켓)
	if (GetMesh())
	{
		FName Weaponsocket(TEXT("Weapon_r"));
		AttackGuardCheckBox->SetupAttachment(GetMesh(), Weaponsocket);

		// 박스 크기 설정
		FVector BoxSize = FVector(10, 10, 10);
		AttackGuardCheckBox->SetBoxExtent(BoxSize);

		// 위치 확인을 위한 로그 추가
		UE_LOG(LogTemp, Display, TEXT("Box attached to socket: %s"), *Weaponsocket.ToString());
		UE_LOG(LogTemp, Display, TEXT("Box world location: %s"), *AttackGuardCheckBox->GetComponentLocation().ToString());
	}

	MaxCombo = 3;
	AttackEndComboState();

	//초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	//초기 상태를 대시 가능으로 설정
	bCanLaunch = true;
	//기본마찰력상태 변수 초기화
	OriginalGroundFriction = GetCharacterMovement()->GroundFriction;

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
	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		// PBox를 CharacterMesh의 ParryCheckBox 소켓에 부착합니다.
		if (CharacterMesh && ParryCheck)
		{
			UE_LOG(LogTemp, Error, TEXT("TEXT"));
			ParryCheck->AttachToComponent(CharacterMesh,FAttachmentTransformRules::KeepRelativeTransform, TEXT("ParryCheckBox"));

			// AttachToComponent 이후에 부모 정보를 출력합니다.
			USceneComponent* PPParentComponent = ParryCheck->GetAttachParent();
			if (PPParentComponent)
			{
				FString ParentName = PPParentComponent->GetName();
				FVector ParentLocation = PPParentComponent->GetComponentLocation();
				FRotator ParentRotation = PPParentComponent->GetComponentRotation();

				UE_LOG(LogTemp, Log, TEXT("Parrycheck is attached to parent: %s at location (%f, %f, %f) and rotation (%f, %f, %f)"),
					*ParentName, ParentLocation.X, ParentLocation.Y, ParentLocation.Z,
					ParentRotation.Roll, ParentRotation.Pitch, ParentRotation.Yaw);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Parrycheck's parent component is null after attaching."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CharacterMesh or PBox is not valid."));
		}
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ParryMontage);
}

//void AHeroCharacter::ParryInput()
//{
//	bIsParryWindow = true;
//
//	UE_LOG(LogTemp, Warning, TEXT("Parry Window Started: %s"), bIsParryWindow ? TEXT("true") : TEXT("false"));
//
//	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &AHeroCharacter::EndParryWindow, 0.2f, false);  // 0.2초 (12프레임) 후 EndParryWindow 호출
//}
//
//void AHeroCharacter::EndParryWindow()
//{
//	bIsParryWindow = false;
//
//	UE_LOG(LogTemp, Warning, TEXT("Parry Window Ended: %s"), bIsParryWindow ? TEXT("true") : TEXT("false"));
//}


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

//void APlayerCharacter::ParryInput()
//{
//	// 패링 상태로 전환
//	bIsParryWindow = true;
//
//	// 패링 상태를 로그로 출력
//	UE_LOG(LogTemp, Warning, TEXT("Parry Window Started: %s"), bIsParryWindow ? TEXT("true") : TEXT("false"));
//
//	// 패링 상태를 일정 시간 후 종료하기 위해 타이머 설정
//	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &APlayerCharacter::EndParryWindow, 0.2f, false); // 0.2초 (12프레임) 후 EndParryWindow 호출
//}
//
//void APlayerCharacter::EndParryWindow()
//{
//	bIsParryWindow = false;
//
//	// 패링 상태 종료를 로그로 출력
//	UE_LOG(LogTemp, Warning, TEXT("Parry Window Ended: %s"), bIsParryWindow ? TEXT("true") : TEXT("false"));
//}

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
		//UE_LOG(LogTemp, Display, TEXT("CurrentCombo = %d"), CurrentCombo);
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
	//UE_LOG(LogTemp, Error, TEXT("upup, Currentcombo = %d"), CurrentCombo);
}

void AHeroCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
	//UE_LOG(LogTemp, Error, TEXT("End, Currentcombo = %d"), CurrentCombo);
}

void AHeroCharacter::DealDamage()
{
	if (AttackGuardCheckBox)
	{
		FVector BoxLocation = AttackGuardCheckBox->GetComponentLocation();
		FVector BoxExtent = AttackGuardCheckBox->GetScaledBoxExtent();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1)); // 예시로 LockOnPawn 타입 추가

		TArray<AActor*> ActorsToIgnore;
		TArray<AActor*> OutActors;

		// BoxOverlapActors 함수를 사용하여 겹치는 액터들을 찾음
		UKismetSystemLibrary::BoxOverlapActors(
			GetWorld(),
			BoxLocation,
			BoxExtent,
			ObjectTypes,
			nullptr, // TraceComplex를 사용하지 않을 경우 nullptr 전달
			ActorsToIgnore,
			OutActors
		);

		// 겹치는 액터들이 있는 경우에만 DrawDebugBox 호출
		for (AActor* OverlappedActor : OutActors)
		{
			FLinearColor LineColor(0, 1, 0, 1);

			// Rotation: 상자의 회전 (기본값은 FRotator::ZeroRotator)
			FRotator Rotation(0, 0, 0);

			// Duration: 디버그 라인이 표시될 시간 (기본값은 2초)
			float Duration = 2.0f;

			// Thickness: 선의 두께 (기본값은 0)
			float Thickness = 0.0f;

			// 각 액터의 위치와 회전에 따라 DrawDebugBox 호출
			FVector ActorLocation = OverlappedActor->GetActorLocation();
			FVector ActorExtent = FVector::ZeroVector; // 액터의 경우, 액터의 크기에 맞게 설정해야 함

			UKismetSystemLibrary::DrawDebugBox(
				GetWorld(),
				ActorLocation,
				BoxExtent,
				LineColor,
				Rotation,
				Duration,
				Thickness
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

//대쉬할떄 위로 살짝 뜨게하는 함수 구현
//void AHeroCharacter::LaunchUpward()
//{
//	//위로 300의 힘으로 이동시킬 변수
//	FVector LaunchVelocity = FVector(0, 0, 300);
//	LaunchCharacter(LaunchVelocity, false, true);
//}

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
//void AHeroCharacter::OnWidget()
//{
//	Hud = CreateWidget<UUserWidget>(GetWorld(), HUDClass);
//	if (Hud != nullptr)
//	{
//		Hud->AddToViewport();
//	}
//}
