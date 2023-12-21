// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Gun.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	GunMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGun::Fire(FVector& StartPoint, FVector& Direction)
{
}


void AGun::Reload()
{
	UE_LOG(LogTemp,Warning, TEXT("RELOAD"));

	int32 ReloadingAmmo = MaxAmmo - CurrentAmmo;

	TotalAmmo -= ReloadingAmmo;

	CurrentAmmo = MaxAmmo;
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

