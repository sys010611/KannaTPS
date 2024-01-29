// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/KannaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/KannaTPSOverlay.h"
#include "Components/ArrowComponent.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	GunMesh->SetupAttachment(GetRootComponent());

	Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	KannaCharacter = Cast<AKannaCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	ExSkillReady = false;
}

void AGun::PlayMuzzleFlashEffect()
{
	if (MuzzleFlashEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(
			MuzzleFlashEffect, Muzzle, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, FVector::One() * 20.f);
	}
}

void AGun::Fire(FVector& StartPoint, FVector& Direction)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo-1, 0, MaxAmmo);

	KannaCharacter->GetKannaTPSOverlay()->SetCurrentAmmoText(CurrentAmmo);
}


void AGun::Reload()
{
	if (MaxAmmo == CurrentAmmo)
		return;

	UE_LOG(LogTemp,Warning, TEXT("RELOAD"));

	int32 ReloadingAmmo = MaxAmmo - CurrentAmmo;

	TotalAmmo -= ReloadingAmmo;

	CurrentAmmo = MaxAmmo;

	KannaCharacter->GetKannaTPSOverlay()->SetCurrentAmmoText(CurrentAmmo);
	KannaCharacter->GetKannaTPSOverlay()->SetTotalAmmoText(TotalAmmo);
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

