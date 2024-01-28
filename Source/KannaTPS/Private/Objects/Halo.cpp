// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Halo.h"

// Sets default values
AHalo::AHalo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHalo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHalo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

