// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunInterface.h"
#include "Gun.generated.h"

class USkeletalMeshComponent;

UCLASS()
class KANNATPS_API AGun : public AActor, public IGunInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire() override;

	virtual void Reload() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 TotalAmmo;


private:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;

};
