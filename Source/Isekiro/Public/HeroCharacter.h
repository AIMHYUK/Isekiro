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

	UPROPERTY(EditAnywhere, Category = "Animation")
    TArray<FName> ParryMontageSections; // Array to hold section names

	
	UPROPERTY(EditAnywhere, Category = "Animation")
    TArray<FName> GuardMontageSections; // Array to hold section names

	UPROPERTY(EditAnywhere, Category = "Animation")
	URealParryBox* ParryCheck;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> CamShake;

	//가드
	bool bIsGuarding = false;
	bool bGuardButtonHold = false;

	void StartGuard(const struct FInputActionValue& Value);
	void EndGuard(const struct FInputActionValue& Value);

	void PlayGuardMontage(FName Section);
	void PlayGuardMontage();
	class UHeroAnimInstance* HeroAnimInstance;

	class UStatusComponent* Status;

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

	//타격감
	void KnockBack(float distance);
	void ShakeCam();

	void ResetCombo();

	FTimerHandle StrongAttackTimerHandle;

	EActionState GetActionState();
	void SetActionStateHazardBegin();
	void SetActionStateHazardEnd();
	void SetActionStateParrySuccess();
	void SetActionStateDifferentWithParry();

	EBossState BossState;
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera;


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

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Guard(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Run(const FInputActionValue& value);
	void Dash(const FInputActionValue& value);
	void UseItem(const FInputActionValue& value);
	void StrongAttack(const FInputActionValue& value);
	void EndStrongAttack();

	
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

	void GetHPPercent();
};


