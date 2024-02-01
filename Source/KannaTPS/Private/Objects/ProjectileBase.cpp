// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Projectile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile"));
	SetRootComponent(Projectile);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(2.f);
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

