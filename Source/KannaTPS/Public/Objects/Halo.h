// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Halo.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class KANNATPS_API AHalo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHalo();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void BreakHalo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UGeometryCollectionComponent* GeometryCollection;
	
	UPROPERTY()
	ACharacter* KannaCharacter;

	FVector TargetLocation;
	FRotator TargetRotation;
};
