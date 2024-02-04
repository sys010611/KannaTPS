// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/ProjectileBase.h"
#include "ExProjectile.generated.h"

class UProjectileMovementComponent;
class ProjectileBase;

UCLASS()
class KANNATPS_API AExProjectile : public AProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Damage(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit) override;

	void SpawnBustedText(const FVector& SpawnLocation, const FVector& InstigatorLocation);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> BustedText;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* HitSound;

	float TextOffset;
};
