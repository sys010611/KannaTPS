// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAttributeComponent;
class UWidgetComponent;
class UHealthBarComponent;
class USoundAttenuation;

UCLASS()
class KANNATPS_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit() override;

	//EEnemyState EnemyState = EEnemyState::EES_Chase;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die(FDamageEvent const& DamageEvent);

	void RagdollEffect(const FDamageEvent& DamageEvent);

	UFUNCTION(BlueprintImplementableEvent)
	void CeaseFire();

	UFUNCTION(BlueprintCallable)
	bool IsDead();

	UFUNCTION(BlueprintCallable)
	void NoticePlayer();

	UFUNCTION(BlueprintCallable)
	void AwarePlayer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundAttenuation* GunfireAttenuation;

private:	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
};
