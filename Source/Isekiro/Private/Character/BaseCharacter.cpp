// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "ActorComponents/StatusComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	StatusComponent = CreateDefaultSubobject<UStatusComponent>("StatusComponent");
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

