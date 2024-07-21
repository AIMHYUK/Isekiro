// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RealHpBar.generated.h"

/**
 * 
 */
UCLASS()
class ISEKIRO_API URealHpBar : public UUserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (BindWidget))
	class UImage* HPBar;

	UPROPERTY(BlueprintReadWrite)
	float ProgressValue;
	UPROPERTY(BlueprintReadWrite)
	float BGStart;
	UPROPERTY(BlueprintReadWrite)
	float BGEnd;
	UPROPERTY(BlueprintReadWrite)
	float FGValueEnd;
	UPROPERTY(BlueprintReadWrite)
	float FGValueStart;

};
