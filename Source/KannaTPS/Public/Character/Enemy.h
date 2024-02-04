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
class AProjectile;
class AGun;

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
	
	UFUNCTION(BlueprintCallable)
	bool IsDead();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die(FDamageEvent const& DamageEvent);

	void RagdollEffect(const FDamageEvent& DamageEvent);

	void OneShot();

	UFUNCTION(BlueprintImplementableEvent)
	void CeaseFire();

	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	void NoticePlayer();

	UFUNCTION(BlueprintCallable)
	void AwarePlayer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundAttenuation* GunfireAttenuation;

	UPROPERTY(BlueprintReadWrite)
	ACharacter* TargetCharacter;


private:	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void PlayHitMontage();

	void PlayMontageBySection(UAnimMontage* Montage, const FName& SectionName);

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, Category = Montage)
	TArray<FName> HitMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* BulletHitSound;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* GunSound;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* BulletStartPos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGun> AssultRifleClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AGun* AssultRifle;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USoundAttenuation* SoundAttenuation;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTimerHandle ShootTimer;
};
