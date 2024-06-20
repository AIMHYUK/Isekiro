// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/FSMComponent.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

