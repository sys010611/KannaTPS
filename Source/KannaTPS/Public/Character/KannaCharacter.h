// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterStates.h"
#include "KannaCharacter.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
struct FInputActionValue;
class UAnimMontage;
class AGun;
class USphereComponent;
struct FEnhancedInputActionValueBinding;
class UKannaTPSOverlay;
class UAttributeComponent;
class UDamageIndicator;


UCLASS()
class KANNATPS_API AKannaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKannaCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	// Getter
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE bool GetIsInCover() const {return IsInCover;}
	FORCEINLINE EAimingDirection GetAimingDirection() const { return AimingDirection; }
	FORCEINLINE bool GetIsReloading() const { return IsReloading; }

	UFUNCTION(BlueprintCallable)
	void SetPunchHitbox(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void SetKickHitbox(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentWeapon(AGun* Weapon) {CurrentWeapon = Weapon; }

	UFUNCTION()
	FORCEINLINE void AddWeaponToList(AGun* Weapon) {WeaponList.Add(Weapon); }

	FORCEINLINE UKannaTPSOverlay* GetKannaTPSOverlay() {return KannaTPSOverlay;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SwitchWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CoverAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SwitchCameraAction;

	FEnhancedInputActionValueBinding* MoveActionBinding;


	/*input callback*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Aim();
	void ReleaseAim();
	void Interact();
	void SwitchWeapon();
	void Attack();
	void Roll();
	void Fire();
	void Reload();
	void TakeCover();


	//Play animation montage
	void PlayAttackMontage();
	void PlayRollMontage();


	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void RollEnd();

	UFUNCTION(BlueprintCallable)
	void ReloadEnd();

	UFUNCTION(BlueprintCallable)
	void FadeOutDamageIndicator();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRollStart();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAimStart();

	UFUNCTION(BlueprintNativeEvent)
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchCameraPos();

	UFUNCTION(BlueprintImplementableEvent)
	void SpreadCrosshair();

	void WallTrace();
	void CoverTrace();
	void CheckLeftRightHit(FVector& WallDirection, FVector& ActorLocation, FHitResult& HitResult, FCollisionQueryParams& CollisionParameters);
	void StartCover(FVector& PlaneNormal, bool IsLowCover);
	void StopCover();

	//캐릭터의 오른쪽에서 정면으로 라인트레이싱 한 결과를 담는 변수
	bool RightHit;
	//캐릭터의 왼쪽에서 정면으로 라인트레이싱 한 결과를 담는 변수
	bool LeftHit;

private:
	void InitKannaTpsOverlay();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void Die();

	FORCEINLINE void DisableMovement() { Controller->SetIgnoreMoveInput(true); }
	void EnableMovement();

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unarmed;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Neutral;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SpringArmDefaultLength;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector SpringArmDefaultOffset;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* StunMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DieMontage;

	UPROPERTY(EditDefaultsOnly, Category = Hitbox)
	USphereComponent* PunchHitbox;

	UPROPERTY(EditDefaultsOnly, Category = Hitbox)
	USphereComponent* KickHitbox;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AGun* CurrentWeapon;

	UPROPERTY()
	TArray<AGun*> WeaponList;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* MeleeAttackSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* BulletHitSound;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsInCover;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsCameraAtRight;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsReloading;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EAimingDirection AimingDirection = EAimingDirection::EAD_Neutral;

	UPROPERTY()
	UKannaTPSOverlay* KannaTPSOverlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDamageIndicator> DamageIndicatorClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UDamageIndicator* DamageIndicator;
};
