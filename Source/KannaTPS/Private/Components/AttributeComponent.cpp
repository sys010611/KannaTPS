// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsRecovering = false;

	CurrentHealth = 100.f;
	MaxHealth = 100.f;

	RecoverRate = 20.f;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (IsRecovering)
	{
		CurrentHealth += DeltaTime * RecoverRate;
		if (CurrentHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
			IsRecovering = false;
		}
	}
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth-Damage, 0.f, 100.f);
}

