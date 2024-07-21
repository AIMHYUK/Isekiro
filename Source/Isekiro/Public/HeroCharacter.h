// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "HeroCharacter.generated.h"

enum class EBossState : uint8;

class UInputMappingContext;
class UCapsuleComponent;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UBoxComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class URealParryBox;
class AIsekiroGameModeBase;
class AHeroPlayerController;

UCLASS()
class ISEKIRO_API AHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AHeroCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//연속공격
	void Attack(const FInputActionValue& value);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY()
	class UHeroAnimInstance* HeroAnim;

	void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void DealDamage();
	//연속공격

	//가드중 이동 속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float GuardWalkSpeed = 200;
	//걷기속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float walkSpeed = 400;
	//달리기속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float runSpeed = 600;
	//앞으로 대쉬 기능 함수 선언
	void LaunchFoward();
	
	//Cooldown초기화 함수 선언
	void ResetLaunchCooldown();

	FTimerHandle DashTimerHandle;
	FTimerHandle ExcuteTimerHandle;
	void StopDash();
	//딜레이 처리할 변수세트 선언
	FTimerHandle LaunchUpwardTimeHandle;
	FTimerHandle CooldownTimerHandle;
	FTimerHandle ResetFrictionTimerHandle;
	//초기 상태를 대시 가능으로 체크할 변수 선언
	bool bCanLaunch;
	//기본 마찰력상태 변수 선언
	float OriginalGroundFriction;
	//마찰력상태 초기화 함수 선언
	void ResetGroundFriction();
	
	//패리
	UFUNCTION()
	void PlayParryMontage();

	void PlayParriedMontage();

	UPROPERTY(EditAnywhere, Category = "Animation")
    TArray<FName> ParryMontageSections; // Array to hold section names

	
	UPROPERTY(EditAnywhere, Category = "Animation")
    TArray<FName> GuardMontageSections; // Array to hold section names

	UPROPERTY(EditAnywhere, Category = "Animation")
    TArray<FName> GetParriedMontageSections; // Array to hold section names
	UPROPERTY(EditAnywhere, Category = "Animation")
	URealParryBox* ParryCheck;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> CamShake;

	//가드
	bool bIsGuarding = false;
	bool bGuardButtonHold = false;

	void StartGuard(const struct FInputActionValue& Value);
	void EndGuard(const struct FInputActionValue& Value);

	/*void PlayGuardMontage(FName Section);*/
	void PlayGuardMontage();
	class UHeroAnimInstance* HeroAnimInstance;

	class UStatusComponent* Status;
	class UStatusComponent* BossStatus;

	AHeroPlayerController* HeroPlayerController;

	AIsekiroGameModeBase* IsekiroGameModeBase;

	//데미지, 체력바
	void ApplyDamage(float damage);
	void ApplyPosture(float Posture);
	void CallHPBarFunction();
	void CallPostureBarFunction();
	UStatusComponent* GetStatusComponent();
	UFUNCTION(BlueprintPure)
	bool IsDead() const;
	bool IsParrying() const;
	//타격감
	void KnockBack(float distance);
	void ShakeCam();

	void ResetCombo();

	FTimerHandle StrongAttackTimerHandle;

	EActionState GetActionState();
	EHazardState GetHazardState();
	void SetActionStateHazardBegin();
	void SetActionStateHazardEnd();
	void SetActionStateParrySuccess();
	void SetActionStateDifferentWithParry();

	EBossState BossState;

	//보스 막타 시간느리게 + 확대
	FTimerHandle TimeDilationHandle;
	FTimerHandle CameraHandle;
	void ResetTimeDilation();
	void MakeSlowTimeForLastBossKill();
	void MakeSlowTimeDilation();
	void MakeCameraDefault();
	bool bIsDilated = false;
	bool bCanExecution = false;

	FTimerHandle DealDamageTimerHandl;

	//연속 점프 막기
	bool bCanJump = true;
	void Landed(const FHitResult& Hit);

	class UAnimInstance* AnimInstance;

	FVector OriginalCameraLocation;
	FRotator OriginalCameraRotation;

	FVector DeathCameraOffset;
	FRotator DeathCameraRotation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBossWidget> BossUI;

	void KillLifePoint();
	FName GetSectionNameFromCombo(int32 ComboNum) const;

	void PutInDamage();

	FTimerHandle MouseChangeHandle;
	
	bool bMousePressed = false;
	UFUNCTION(BlueprintPure)
	bool GetMousePressed();
	void ChangeMousePressed();

	UFUNCTION(BlueprintCallable)
	float easeInCubic(float x)
	{
		return x * x * x;
	}
	UFUNCTION(BlueprintCallable)
	float easeOutQuint(float x) {
		return 1 - FMath::Pow(1 - x, 5);
	}
	UFUNCTION(BlueprintCallable)
	float easeInCirc(float x){
		return 1 - FMath::Sqrt(1 - FMath::Pow(x, 2));
	}

	void ReadyToAttack();

	bool bIsMouseRightClicking = false;
	FTimerHandle MontageTimerHandle;

	// 타이머 완료 시 호출될 함수 선언
	void StopMontage();
	
	void PlayGuardSoonStopMontage();
	
	//UFUNCTION(BlueprintCallable, Category = "Camera")
	//void SwitchToCineCamera();
protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* DeathCamera;


	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* Context;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RunAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* GuardAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DashAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* UseItemAction;	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* StrongAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GuardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* KeepGuardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ParryMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HittedMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HittedWhileGuardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DefenseBreakMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* StrongAttackMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ExecutionMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GetParriedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SlideMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackReadyMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GameFinishAttackMontage;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Guard(const FInputActionValue& value);
	void HeroJump(const FInputActionValue& value);
	void Run(const FInputActionValue& value);
	void Dash();
	void UseItem(const FInputActionValue& value);
	void StrongAttack();
	void EndStrongAttack();

	UFUNCTION()
	void Die();
	
	UFUNCTION()
	void PlayHittedMontage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnHittedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHittedWhileGuardMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	bool bIsAttacking = false;
	bool bContinueAttack = false;
	bool bIsAttacked = false;

private:
	ECharacterGuardState GuardState = ECharacterGuardState::ECGS_UnGuarded;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ECharacterGuardState GetCharacterState() const { return GuardState; }

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EHazardState HazardState = EHazardState::EHS_NoHazard;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bHasDisplayedHazardUI;	

	void GetHPPercent();
	bool IsBossPostureBroken();

};


