// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/AssultRifle.h"

// Sets default values
AAssultRifle::AAssultRifle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireMode = EFireMode::EFM_AUTO;
}

// Called when the game starts or when spawned
void AAssultRifle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAssultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

