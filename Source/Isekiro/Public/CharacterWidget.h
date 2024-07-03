// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class ISEKIRO_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* HPBar;	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* PostureBar;

	UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateProgressBar();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    class UStatusComponent* StateComponent;

};
