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
#include "ActorComponents/StatusComponent.h"
#include "IsekiroGameModeBase.h"
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

	Status = CreateDefaultSubobject<UStatusComponent>(TEXT("Status"));
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
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap); //캡슐콜리전이 attackbox감지
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHeroCharacter::PlayHittedMontage); //오버랩되면  함수실행
	}

	if (PlayerController)
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			Status = PlayerPawn->FindComponentByClass<UStatusComponent>();
			Status->StartPostureRecovery();
			UE_LOG(LogTemp, Display, TEXT("ENHANCED"));
			if (!Status)
			{
				UE_LOG(LogTemp, Warning, TEXT("UStatusComponent not found on PlayerPawn"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerPawn not found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not found"));
	}
	IsekiroGameModeBase = Cast<AIsekiroGameModeBase>(GetWorld()->GetAuthGameMode());

	//CallHPBarFunction();
	//CallPostureBarFunction();
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
			UE_LOG(LogTemp, Error, TEXT("ATTackHitCheck"));
		});	
}
// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HeroAnimInstance = Cast<UHeroAnimInstance>(GetMesh()->GetAnimInstance());
	if (HeroAnimInstance->Montage_IsPlaying(HeroAnimInstance->GuardMontage))
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("is playing %s"), *GetNameSafe(HeroAnimInstance->GuardMontage)));
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("is playinh????")));;

}


void AHeroCharacter::ApplyDamage(float damage)
{
	Status->ApplyHealthDamage(damage);

}

void AHeroCharacter::ApplyPosture(float posture)
{
	Status->ApplyPostureDamage(posture);
}

void AHeroCharacter::CallHPBarFunction()
{
	IsekiroGameModeBase->UpdateHPBar();
}

void AHeroCharacter::CallPostureBarFunction()
{
	IsekiroGameModeBase->UpdatePostureBar();
}

UStatusComponent* AHeroCharacter::GetStatusComponent()
{
	return Status;
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
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Triggered, this, &AHeroCharacter::StartGuard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &AHeroCharacter::EndGuard);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AHeroCharacter::AttackStart);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Run);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AHeroCharacter::Dash);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &AHeroCharacter::UseItem);
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
//void AHeroCharacter::StartGuard(const FInputActionValue& Value)
//{
//	bIsGuarding = true;
//	bGuardButtonHold = true;
//
//	PlayGuardMontage(FName("StartGuard"));
//	UE_LOG(LogTemp, Display, TEXT("Startguard"));
//	
//}
void AHeroCharacter::EndGuard(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Endguard"));
	GuardState = ECharacterGuardState::ECGS_UnGuarded;
	auto Movement = GetCharacterMovement();
	//가드걷기속도로 전환
	Movement->MaxWalkSpeed = walkSpeed;
}

void AHeroCharacter::PlayGuardMontage(FName SectionName)
{
	HeroAnimInstance = Cast<UHeroAnimInstance>(GetMesh()->GetAnimInstance());
	if (HeroAnimInstance && HeroAnimInstance->GuardMontage)
	{
		// Guard 몽타주가 이미 재생 중이 아닌 경우에만 재생
		if (!HeroAnimInstance->Montage_IsPlaying(HeroAnimInstance->GuardMontage))
		{
			HeroAnimInstance->Montage_Play(HeroAnimInstance->GuardMontage);
			HeroAnimInstance->Montage_JumpToSection(SectionName, HeroAnimInstance->GuardMontage);
		}

	}
}

void AHeroCharacter::StartGuard(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	ParryCheck->ParryStarted();
	GuardState = ECharacterGuardState::ECGS_Guarding;
	
	auto Movement = GetCharacterMovement();
	//가드걷기속도로 전환
	Movement->MaxWalkSpeed = GuardWalkSpeed;

}

void AHeroCharacter::PlayGuardMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(GuardMontage);
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
			ShakeCam();
			KnockBack(500);
			// Log the selected section (optional, for debugging)
			UE_LOG(LogTemp, Log, TEXT("Playing Parry Montage Section: %s"), *SelectedSection.ToString());
		}
		PlayerController->SetIgnoreMoveInput(false);

}
void AHeroCharacter::KnockBack(float Distance) // 뒤로 밀리는 함수
{
	GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
	FVector KnockBackDirection = -GetActorForwardVector();

	LaunchCharacter(KnockBackDirection * Distance, true, true);
}
void AHeroCharacter::ShakeCam()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager)
	{
		// 클라이언트에서 카메라 흔들림 효과 시작
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientStartCameraShake(CamShake);
		}

		// 월드 공간에서 카메라 흔들림 효과 적용
		CameraManager->PlayWorldCameraShake(GetWorld(), CamShake, GetActorLocation(), 0.0f, 50.0f, 1.0f);
	}
}

void AHeroCharacter::PlayHittedMontage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	HeroAnimInstance = Cast<UHeroAnimInstance>(AnimInstance);
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UE_LOG(LogTemp, Display, TEXT("%f"), Status->GetPosture());
	
	bool bPostureExceeded = false; //체간이 100을 넘었을때 한대 더 맞으면 가드 브레이크
	if (Status->GetPosture() > 100)
		bPostureExceeded = true;
	if (bPostureExceeded)
	{
		PlayerController->SetIgnoreMoveInput(true);
		AnimInstance->Montage_Play(DefenseBreakMontage);
		Status->SetPostureZero();
		PlayerController->SetIgnoreMoveInput(false);
	}
	else
	{
		if (GuardState == ECharacterGuardState::ECGS_UnGuarded && !HeroAnimInstance->Montage_IsPlaying(ParryMontage)) //가드를 내리고 있고 패링 몽타주가 실행되고 있지 않다면
		{
			PlayerController->SetIgnoreMoveInput(true); //못 움직이게 하고 맞는 판정
			
			if (AnimInstance)
			{
				ApplyDamage(10);
				ApplyPosture(10); 
				AnimInstance->Montage_Play(HittedMontage);
				// 몽타주가 끝났을 때 호출될 델리게이트 설정
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &AHeroCharacter::OnHittedMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HittedMontage);
				UE_LOG(LogTemp, Display, TEXT("HP : %f"), Status->GetHealth())
			}
		}
		else if (!HeroAnimInstance->Montage_IsPlaying(ParryMontage)) //패링이 실패했다면 그냥 가드 모션 띄우기
		{
			GuardMontageSections = { TEXT("DefenseHit1"), TEXT("DefenseHit2"), TEXT("DefenseHit3"), TEXT("DefenseHit4") }; //섹션 이름 받아서
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(HittedWhileGuardMontage);
				if (GuardMontageSections.Num() > 0)
				{
					ApplyDamage(4);
					ApplyPosture(10);
					UE_LOG(LogTemp, Error, TEXT("HIt while guard"));
					int32 GuardSectionIndex = FMath::RandRange(0, GuardMontageSections.Num() - 1);
					FName GuardSelectedSection = GuardMontageSections[GuardSectionIndex]; //랜덤하게 플레이하기
					AnimInstance->Montage_JumpToSection(GuardSelectedSection, HittedWhileGuardMontage);
					KnockBack(300);

				}
			}
		}
	}
	//CallHPBarFunction();
	//CallPostureBarFunction();
}

void AHeroCharacter::OnHittedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(false); // 몽타주가 끝난 후 이동 재활성화
	}
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
	if (IsAttacking)
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) //다른 몽타주 재생중일 때 실행 불가능
		{
			DealDamage();
			AttackStartComboState();
			HeroAnim->PlayAttackMontage();
			HeroAnim->JumpToAttackMontageSection(CurrentCombo);
			IsAttacking = true;
		}
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
			if (OverlappedActor) // 액터 유효성 확인
			{
				// State 컴포넌트를 가져옴
				Status = OverlappedActor->FindComponentByClass<UStatusComponent>();
				if (Status)
				{
					Status->ApplyHealthDamage(10);
					Status->ApplyPostureDamage(10);
					UE_LOG(LogTemp, Display, TEXT("HP %f"), Status->GetHealth());
				}
			}
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

void AHeroCharacter::UseItem(const FInputActionValue& value)
{
	Status->UsePortion();
}

//앞으로 대쉬 기능 함수 구현
void AHeroCharacter::LaunchFoward()
{
	//앞으로 이동시킬 Velocity
	FVector LaunchVelocity = GetVelocity().GetSafeNormal() * 900;
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

