// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Pistol.h"
#include "Character/Enemy.h"
#include "Perception/AISense_Hearing.h"
#include "Character/KannaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/KannaTPSOverlay.h"
#include "Objects/ExProjectile.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Character/Enemy.h"
#include "GameFramework/HUD.h"
#include "HUD/KannaTPSHUD.h"
#include "HUD/KannaTPSOverlay.h"

// Sets default values
APistol::APistol()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FireMode = EFireMode::EFM_SEMIAUTO;

	Range = 10000.f;
	Damage = 50.f;

	MaxAmmo = 17;

	HasExSkill = true;
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();

	if (KannaCharacter)
	{
		KannaCharacter->AddWeaponToList(this); // 拳銃は開始時から所持している必要がある。

		this->AttachToComponent(KannaCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Pistol_Socket"));
	}

	TotalAmmo = 500;
	CurrentAmmo = 17;

	GunMesh->SetVisibility(false);
	OnVisibilityChanged(false);
}

void APistol::ReadyExSkill()
{
	Super::ReadyExSkill();

	// マズルに光が集まるエフェクト
	if (ExChargeEffect)
	{
		ExChargeEffectComp = UGameplayStatics::SpawnEmitterAttached(
			ExChargeEffect, Muzzle, NAME_None, FVector::ZeroVector, FRotator(90.f, 0.f, 0.f), FVector::One() * 10);
	}
}


// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::Fire(FVector& StartPoint, FVector& Direction)
{
	Super::Fire(StartPoint, Direction);

	FHitResult HitResult;
	FVector EndPoint = StartPoint + Direction * Range;

	if (GetWorld())
	{
		if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartPoint,
			EndPoint,
			ECollisionChannel::ECC_Visibility))
		{
			if (HitResult.GetActor())
			{
				if (ExSkillReady && ExProjectileClass)
				{
					FireExSkill(HitResult);
				}
				else
				{
					if (IHitInterface* HitObject = Cast<IHitInterface>(HitResult.GetActor()))
					{
						HitObject->GetHit();

						if (Cast<AEnemy>(HitObject))
						{
							APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
							AKannaTPSHUD* KannaTPSHUD = Cast<AKannaTPSHUD>(PlayerController->GetHUD());
							if (KannaTPSHUD)
							{
								KannaTPSHUD->GetKannaTPSOverlay()->ShowHitMarker();
							}
						}
					}

					UGameplayStatics::ApplyPointDamage(
						HitResult.GetActor(),
						Damage,
						Direction,
						HitResult,
						GetInstigator()->GetController(),
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}
	}

	UAnimInstance* AnimInstance = GunMesh->GetAnimInstance();
	if (SlideMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(SlideMontage);
	}

	// マズルフラッシュ効果
	PlayMuzzleFlashEffect();


	// 騒音効果
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.f,
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0),
		6000.f
	);
}

void APistol::FireExSkill(const FHitResult& HitResult)
{
	ExChargeEffectComp->Deactivate();

	// 投射物をスポーンする
	FVector SpawnLocation = Muzzle->GetComponentLocation();
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, HitResult.ImpactPoint);

	FActorSpawnParameters Param;
	Param.Instigator = GetInstigator();
	GetWorld()->SpawnActor<AExProjectile>(ExProjectileClass, SpawnLocation, SpawnRotation, Param);

	ExSkillReady = false;
}

