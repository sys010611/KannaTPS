// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KANNATPS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReceiveDamage(float Damage);
	
	FORCEINLINE void EnableHealthRegen() { IsRecovering = true;};
	FORCEINLINE void DisableHealthRegen() { IsRecovering = false; };
	FORCEINLINE float GetCurrentHealth() {return CurrentHealth;}
	FORCEINLINE float GetExGaugePercent() { return ExGaugePercent; }
	FORCEINLINE void SubtractExGaugePercent(float Amount) {ExGaugePercent -= Amount;}
	FORCEINLINE bool IsDead() { return CurrentHealth == 0; }


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float RecoverRate;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float ExRegenRate;

	float ExGaugePercent;

	bool IsRecovering;
};
