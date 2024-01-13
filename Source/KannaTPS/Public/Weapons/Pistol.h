// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.h"
#include "Pistol.generated.h"

class AKannaCharacter;

UCLASS()
class KANNATPS_API APistol : public AGun
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APistol();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire(FVector& StartPoint, FVector& Direction) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SlideMontage;
};
