// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class ISEKIRO_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage *DeathImage;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage *BackGround;
	
};
