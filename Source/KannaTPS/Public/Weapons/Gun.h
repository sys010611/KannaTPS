// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class USkeletalMeshComponent;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 TotalAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 MaxAmmo;

	UPROPERTY()
	EFireMode FireMode;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere)
	float Range;

	UPROPERTY(VisibleAnywhere)
	float Damage;

private:


};
