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
#include "FSM/FSMComponent.h"
#include "Character/BossCharacter.h"
#include "BossWidget.h"

// Sets default values
AHeroCharacter::AHeroCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm); //생성자에서 만들자..

	DeathCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCamera")); //죽었을때 카메라
	DeathCamera->SetupAttachment(GetRootComponent());
	DeathCamera->bAutoActivate = false; // 초기에는 비활성화

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

	DeathCameraOffset = FVector(0.0f, 0.0f, 300.0f); // 캐릭터 위쪽으로 300 유닛
	DeathCameraRotation = FRotator(-45.0f, 0.0f, 0.0f); // 아래쪽을 향하도록 45도 회전

	bHasDisplayedHazardUI = false;
}

EActionState AHeroCharacter::GetActionState()
{
	return ActionState;
}

EHazardState AHeroCharacter::GetHazardState()
{
	return HazardState;
}

void AHeroCharacter::SetActionStateHazardBegin()
{
	HazardState = EHazardState::EHS_Hazard;
}

void AHeroCharacter::SetActionStateHazardEnd()
{
	HazardState = EHazardState::EHS_NoHazard;
}

void AHeroCharacter::SetActionStateParrySuccess()
{
	ActionState = EActionState::EAS_ParrySuccess;
}

void AHeroCharacter::SetActionStateDifferentWithParry()
{
	ActionState = EActionState::EAS_Attacking;
}

void AHeroCharacter::KillLifePoint()
{
	UBossWidget* BW = IsekiroGameModeBase->GetBossUI();
	if (BW)
	{
			BW->DisplayLoseLifePoint(true);		
	}
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

	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(this, 0); // 게임 시작 시 기본 카메라 설정
	}

	// Ensure the owner is valid and has a mesh component
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	ParryCheck->AttachToComponent(CharacterMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("FX_weapon_base"));
	if (ParryCheck)
	{
		UE_LOG(LogTemp, Display, TEXT("ParryChecked"));
	}
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

	AnimInstance = GetMesh()->GetAnimInstance();


}

void AHeroCharacter::PostInitializeComponents() //생성자 비스무리한거 그런느낌
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
		});
}
// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Status->RecoverPosture(2.f);

	switch (ActionState)
	{
	case EActionState::EAS_Attacking:
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("Attacking")));
		break;
	case EActionState::EAS_ParrySuccess:
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("ParrySuccess")));
		break;
	}

	HeroAnimInstance = Cast<UHeroAnimInstance>(GetMesh()->GetAnimInstance());
	if (HeroAnimInstance)
	{
		// 현재 재생 중인 몽타주를 가져옴
		UAnimMontage* CurrentMontage = HeroAnimInstance->GetCurrentActiveMontage();

		if (CurrentMontage)
		{
			// 재생 중인 몽타주가 있으면 그 이름을 출력
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("Currently playing: %s"), *GetNameSafe(CurrentMontage)));
		}
		else
		{
			// 재생 중인 몽타주가 없으면 메시지를 출력
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, TEXT("No montage playing"));
		}
	}
	else
	{
		// HeroAnimInstance가 유효하지 않은 경우 에러 메시지 출력
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, TEXT("HeroAnimInstance is not valid"));
	}

	if (!bHasDisplayedHazardUI && GetHazardState() == EHazardState::EHS_Hazard)
	{
		bHasDisplayedHazardUI = true;
	}
	else if (bHasDisplayedHazardUI && GetHazardState() == EHazardState::EHS_NoHazard)
	{
		bHasDisplayedHazardUI = false;
	}

	GetHazardState() == EHazardState::EHS_Hazard ?
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("HAZARD")))
		:
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("NO HAZARD")));
}

void AHeroCharacter::ApplyDamage(float damage)
{
	Status->TryApplyDamage(0, damage);
	if (IsDead())
	{
		DetachFromControllerPendingDestroy(); //컨트롤러 떼기
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AHeroCharacter::ApplyPosture(float posture)
{
	Status->TryApplyDamage(posture, 0);
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

bool AHeroCharacter::IsDead() const
{
	return (Status->GetHealth()) <= 0;
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &AHeroCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AHeroCharacter::StrongAttack);
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
	if (!IsAttacking && bCanJump)
	{
		Super::ACharacter::Jump();
		bCanJump = false;
		//GetCharacterMovement()->JumpZVelocity = 800.0f;//800으로 임의 상승
		//GetCharacterMovement()->GravityScale = 2.0f; //중력 2배
		/*UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Hello", true, true, FLinearColor(0.0f, 0.66f, 1.0f, 1.0f), 2.0f);*/
	}
}
void AHeroCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bCanJump = true;
	UE_LOG(LogTemp, Error, TEXT("Landed"));
}


void AHeroCharacter::EndGuard(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Endguard"));
	GuardState = ECharacterGuardState::ECGS_UnGuarded;
	auto Movement = GetCharacterMovement();
	//가드걷기속도로 전환
	Movement->MaxWalkSpeed = walkSpeed;
}

//void AHeroCharacter::PlayGuardMontage(FName SectionName)
//{
//	HeroAnimInstance = Cast<UHeroAnimInstance>(GetMesh()->GetAnimInstance());
//	if (HeroAnimInstance && HeroAnimInstance->GuardMontage)
//	{
//		// Guard 몽타주가 이미 재생 중이 아닌 경우에만 재생
//		if (!HeroAnimInstance->Montage_IsPlaying(HeroAnimInstance->GuardMontage))
//		{
//			HeroAnimInstance->Montage_Play(HeroAnimInstance->GuardMontage);
//			HeroAnimInstance->Montage_JumpToSection(SectionName, HeroAnimInstance->GuardMontage);
//		}
//
//	}
//}

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
	if (!AnimInstance->Montage_IsPlaying(DefenseBreakMontage)) //가드브레이크는 출력되야함
	{
		AnimInstance->Montage_Play(GuardMontage);
	}
}

void AHeroCharacter::PlayParryMontage()
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetIgnoreMoveInput(true); //패링중 못움직이게

	ParryMontageSections = { TEXT("Parry1"), TEXT("Parry2"), TEXT("Parry3") }; //섹션 이름 받아서


	AnimInstance->Montage_Play(ParryMontage);
	if (ParryMontageSections.Num() > 0)
	{
		// Select a random section from the array
		int32 SectionIndex = FMath::RandRange(0, ParryMontageSections.Num() - 1);
		FName SelectedSection = ParryMontageSections[SectionIndex]; //랜덤하게 플레이하기

		// Jump to the selected section
		AnimInstance->Montage_JumpToSection(SelectedSection, ParryMontage);
		ShakeCam();
		// Log the selected section (optional, for debugging)
		UE_LOG(LogTemp, Log, TEXT("Playing Parry Montage Section: %s"), *SelectedSection.ToString());
	}
	PlayerController->SetIgnoreMoveInput(false);

}
void AHeroCharacter::PlayParriedMontage()
{ //by justin
	BossState = UGameplayStatics::GetActorOfClass(this, ABossCharacter::StaticClass())->GetComponentByClass<UFSMComponent>()->GetCurrentStateE();
	if (BossState == EBossState::PARRY)
	{
		if (!AnimInstance->Montage_IsPlaying(GetParriedMontage))
			ResetCombo();
			AnimInstance->Montage_Play(GetParriedMontage);
	}
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
		CameraManager->PlayWorldCameraShake(GetWorld(), CamShake, GetActorLocation(), 0.0f, 5.0f, 1.0f);
	}
}
void AHeroCharacter::ResetCombo()
{
	CurrentCombo = 0;
}

void AHeroCharacter::StrongAttack(const FInputActionValue& value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && !(HeroAnim->Montage_IsPlaying(GuardMontage)))
	{
		PlayerController->SetIgnoreMoveInput(true);
		if (AnimInstance && StrongAttackMontage)
		{
			AnimInstance->Montage_Play(StrongAttackMontage);

		}

		GetWorldTimerManager().SetTimer(StrongAttackTimerHandle, this, &AHeroCharacter::EndStrongAttack, 1.4f, false);
	}
}

void AHeroCharacter::EndStrongAttack()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DealDamage();
		PlayerController->SetIgnoreMoveInput(false);

	}
}

void AHeroCharacter::Die()
{
	if (IsDead())
	{
		UE_LOG(LogTemp, Display, TEXT("DEAD"));
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->SetViewTargetWithBlend(this, 0.5f); // 사망 카메라로 전환
			DeathCamera->SetActive(true);
			Camera->SetActive(false);
		}
	}


}

void AHeroCharacter::PlayHittedMontage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	HeroAnimInstance = Cast<UHeroAnimInstance>(AnimInstance);
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UE_LOG(LogTemp, Display, TEXT("%f"), Status->GetPosture());
	//Status->OnStatusChanged.AddDynamic(this, Die); 아쉽지만..

	if (Status->GetPosture() >= 100 && !(AnimInstance->Montage_IsPlaying(ParryMontage))) //체간이 100을 넘기고 패링에 실패했다면 
	{
		AnimInstance->Montage_Play(DefenseBreakMontage);
		Status->SetPostureZero();
	}
	else
	{
		if (GuardState == ECharacterGuardState::ECGS_UnGuarded && !HeroAnimInstance->Montage_IsPlaying(ParryMontage)) //가드를 내리고 있고 패링 몽타주가 실행되고 있지 않다면
		{
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true); //못 움직이게 하고 맞는 판정

			if (AnimInstance)
			{
				if (GetHazardState() == EHazardState::EHS_Hazard)
				{
					ApplyDamage(30);
					ApplyPosture(30);
				}
				else
				{
					ApplyDamage(20);
					ApplyPosture(20);
				}

				AnimInstance->Montage_Play(HittedMontage);
				// 몽타주가 끝났을 때 호출될 델리게이트 설정
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &AHeroCharacter::OnHittedMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HittedMontage);
				ResetCombo();
			}

			PlayerController->SetIgnoreMoveInput(false);
			PlayerController->SetIgnoreLookInput(false);
		}
		else if (!AnimInstance->Montage_IsPlaying(ParryMontage) && !(AnimInstance->Montage_IsPlaying(DefenseBreakMontage))) //패링에 실패하고 가드브레이크가 안터졌으면 가드
		{
			GuardMontageSections = { TEXT("DefenseHit1"), TEXT("DefenseHit2"), TEXT("DefenseHit3"), TEXT("DefenseHit4") }; //섹션 이름 받아서
			if (AnimInstance)
			{
				AnimInstance->OnMontageEnded.AddDynamic(this, &AHeroCharacter::OnHittedWhileGuardMontageEnded);
				AnimInstance->Montage_Play(HittedWhileGuardMontage);
				BossState = UGameplayStatics::GetActorOfClass(this, ABossCharacter::StaticClass())->GetComponentByClass<UFSMComponent>()->GetCurrentStateE();
				if (GuardMontageSections.Num() > 0) //랜덤한 가드히트모션 
				{
					ApplyDamage(0);
					ApplyPosture(10);
					UE_LOG(LogTemp, Error, TEXT("HIt while guard"));
					int32 GuardSectionIndex = FMath::RandRange(0, GuardMontageSections.Num() - 1);
					FName GuardSelectedSection = GuardMontageSections[GuardSectionIndex]; //랜덤하게 플레이하기
					AnimInstance->Montage_JumpToSection(GuardSelectedSection, HittedWhileGuardMontage);
					switch (BossState)
					{
					case EBossState::LUNGEATTACK:
						KnockBack(3000);
						break;
					case EBossState::JUMPATTACK:
						KnockBack(3000);
						break;
					case EBossState::THRUSTATTACK:
						GuardState = ECharacterGuardState::ECGS_GuardBroken;
						AnimInstance->Montage_Play(DefenseBreakMontage);
						break;
					case EBossState::DISTANCEATTACK:
						GuardState = ECharacterGuardState::ECGS_GuardBroken;
						AnimInstance->Montage_Play(DefenseBreakMontage);
						break;
					default:
						KnockBack(800);
						break;
					}
					ResetCombo();
				}
			}
		}
	}
	if (IsDead())
	{
		Die();
		IsekiroGameModeBase->PlayerIsDead();
	}
	//CallHPBarFunction();
	//CallPostureBarFunction();
}
void AHeroCharacter::OnHittedWhileGuardMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HittedWhileGuardMontage)
	{
		PlayGuardMontage();

	}
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
	else if (!AnimInstance->Montage_IsPlaying(GuardMontage))
	{
		//달리기속도로 전환
		movement->MaxWalkSpeed = runSpeed;
	}
}

void AHeroCharacter::Attack(const FInputActionValue& value)
{
	if (IsBossPostureBroken()) //체간이 무너지면
	{
		//시간을 느리게 함
		//입력을 받으면 시간이 다시 빨라짐
		//입력은 bool값으로 구분 
		ABossCharacter* BossDie = Cast<ABossCharacter>(UGameplayStatics::GetActorOfClass(this, ABossCharacter::StaticClass()));
		if (bIsDilated)
		{
			if (bCanExecution)
			{
				Camera->SetFieldOfView(70.f);
				GetWorld()->GetTimerManager().ClearTimer(TimeDilationHandle);
				ResetTimeDilation();

				if (BossDie)
				{
					UFSMComponent* FSM = Cast<UFSMComponent>(BossDie->GetComponentByClass(UFSMComponent::StaticClass()));
					if (FSM)
					{
						FSM->RespondToInput(); //보스의 죽을준비
					}
				}
				AnimInstance->Montage_Play(ExecutionMontage);
				GetWorld()->GetTimerManager().SetTimer(CameraHandle, this, &AHeroCharacter::MakeCameraDefault, 2.f, false);
			}
		}
	}
	else
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
			if (!HeroAnim->Montage_IsPlaying(GuardMontage) && !HeroAnim->Montage_IsPlaying(GetParriedMontage))
			{
				if (!AnimInstance->Montage_IsPlaying(GetParriedMontage))
				{
					AttackStartComboState();
					bool bIsPlaying = HeroAnim->Montage_IsPlaying(AttackMontage);
					if (!bIsPlaying && !(HeroAnim->Montage_IsPlaying(GuardMontage)))
					{
						DealDamage();
						HeroAnim->PlayAttackMontage();
						HeroAnim->JumpToAttackMontageSection(CurrentCombo);
						IsAttacking = true;
					}
				}
			}
		}
	}
}
FName AHeroCharacter::GetSectionNameFromCombo(int32 ComboNum) const
{
	switch (ComboNum)
	{
	case 1:
		return FName("Attack1");
	case 2:
		return FName("Attack2");
	case 3:
		return FName("Attack3");
	default:
		return FName(""); // 예기치 않은 값이면 빈 이름 반환
	}
}

void AHeroCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	AttackEndComboState();
	UE_LOG(LogTemp, Error, TEXT("onEnded, Currentcombo = %d"), CurrentCombo);
}

void AHeroCharacter::AttackStartComboState()
{

	CanNextCombo = true;
	IsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	UE_LOG(LogTemp, Error, TEXT("StartCombo, Currentcombo = %d"), CurrentCombo);
	
}

void AHeroCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}


void AHeroCharacter::DealDamage()
{
	// 이미 데미지를 입은 액터들을 추적하기 위한 맵
	static TMap<AActor*, bool> DamagedActors;

	if (ParryCheck)
	{
		FVector SphereLocation = ParryCheck->GetComponentLocation();
		float SphereSize = ParryCheck->GetScaledSphereRadius();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1)); // 예시로 LockOnPawn 타입 추가

		TArray<AActor*> ActorsToIgnore;
		TArray<AActor*> OutActors;

		DrawDebugSphere(GetWorld(), SphereLocation, SphereSize, 12, FColor::Red, false, 5.0f);

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
			UE_LOG(LogTemp, Error, TEXT("Overllaped"));
			if (OverlappedActor && !DamagedActors.Contains(OverlappedActor)) // 액터 유효성 확인 및 이미 데미지를 입었는지 체크
			{
				UE_LOG(LogTemp, Error, TEXT("Overllaped"));
				// State 컴포넌트를 가져옴
				UStatusComponent* ActorStatus = OverlappedActor->FindComponentByClass<UStatusComponent>();
				if (ActorStatus)
				{
					UE_LOG(LogTemp, Display, TEXT("OverlappedActor : %s"), *OverlappedActor->GetName());
					// 액터를 이미 데미지를 입은 것으로 표시			
					DamagedActors.Add(OverlappedActor, true);
					if (ActorStatus->TryApplyDamage(10, 10) &&  IsBossPostureBroken()) //체간이 무너지면
					{
						MakeSlowTimeDilation();
					}
				}
			}
		}
		// 맵 초기화 (다음 프레임에 다시 데미지를 줄 수 있도록)
		DamagedActors.Empty();
	}
}
void AHeroCharacter::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
	bIsDilated = false;
	bCanExecution = false;

}

void AHeroCharacter::MakeSlowTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 0.05f);
	GetWorld()->GetTimerManager().SetTimer(TimeDilationHandle, this, &AHeroCharacter::ResetTimeDilation, 2.f, false);
	bIsDilated = true;
	bCanExecution = true;
}

void AHeroCharacter::MakeCameraDefault()
{
	Camera->SetFieldOfView(90.f);
}

bool AHeroCharacter::IsBossPostureBroken()
{
	ABossCharacter* Boss = Cast<ABossCharacter>(UGameplayStatics::GetActorOfClass(this, ABossCharacter::StaticClass()));
	UStatusComponent* BStatus = Cast<UStatusComponent>(Boss->GetComponentByClass(UStatusComponent::StaticClass()));
	if (Boss)
	{
		UFSMComponent* FSMComponent = Cast<UFSMComponent>(Boss->GetComponentByClass(UFSMComponent::StaticClass()));
		if (FSMComponent)
		{
			return (FSMComponent->IsPostureBroken() || BStatus->GetHealth() <= 0);
		}
	}
	return false;
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

