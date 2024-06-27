// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "HeroCharacter.generated.h"

class UInputMappingContext;
class UCapsuleComponent;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UBoxComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;


UCLASS()
class ISEKIRO_API AHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AHeroCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//연속공격
	void AttackStart(const FInputActionValue& value);

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
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, Category = "Components")
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* AttackGuardCheckBox;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInterface* BaseMaterial; 


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GuardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* KeepGuardMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Guard(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);

	//void Attack(const FInputActionValue& value);

	//UFUNCTION(BlueprintCallable)
	/*bool CanAttack();*/

	bool bIsAttacking = false;
	bool bContinueAttack = false;
private:
	ECharacterGuardState GuardState = ECharacterGuardState::ECGS_UnGuarded;

	FORCEINLINE ECharacterGuardState GetCharacterState() const { return GuardState; }

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;




};
