// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class USkeletalMeshComponent;
class UArrowComponent;
class AKannaCharacter;

UENUM()
enum class EFireMode
{
	EFM_SEMIAUTO,
	EFM_AUTO
};

UCLASS(Abstract)
class KANNATPS_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Fire(FVector& StartPoint, FVector& Direction);
	virtual void Reload();

	FORCEINLINE bool IsShootable() {return CurrentAmmo > 0;}
	FORCEINLINE bool IsReloadable() {return CurrentAmmo < MaxAmmo;}
	FORCEINLINE bool IsExSkillReady() { return ExSkillReady; }
	FORCEINLINE int32 GetCurrentAmmo() {return CurrentAmmo;}
	FORCEINLINE int32 GetTotalAmmo() { return TotalAmmo; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() { return GunMesh; }
	FORCEINLINE UArrowComponent* GetMuzzle() {return Muzzle;}
	FORCEINLINE virtual void ReadyExSkill() {ExSkillReady = true;}

	// 銃器を非表示にしたときのイベント。マガジンも一緒に隠す役割をする。
	UFUNCTION(BlueprintImplementableEvent)
	void OnVisibilityChanged(bool Visibility);

	UFUNCTION(BlueprintCallable)
	void PlayMuzzleFlashEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UArrowComponent* Muzzle;

	bool HasExSkill;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AKannaCharacter* KannaCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 TotalAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 MaxAmmo;

	UPROPERTY()
	EFireMode FireMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere)
	float Range;

	UPROPERTY(VisibleAnywhere)
	float Damage;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = VFX)
	UParticleSystem* MuzzleFlashEffect;

	bool ExSkillReady;

private:

};
