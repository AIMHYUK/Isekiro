// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossWidget.generated.h"

class UProgressBar;
class UImage;
/**
 * 
 */
UCLASS()
class ISEKIRO_API UBossWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BossHPBar;	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BossPostureBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	void DisplayPostureBroken(bool bDisplay);
	void DisplayLoseLifePoint(bool bCanLoseLife);

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> Image_PostureBreak;
};
