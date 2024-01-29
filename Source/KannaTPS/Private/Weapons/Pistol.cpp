// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Pistol.h"
#include "Character/Enemy.h"
#include "Perception/AISense_Hearing.h"
#include "Character/KannaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/KannaTPSOverlay.h"
#include "Objects/Projectile.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FireMode = EFireMode::EFM_SEMIAUTO;

	Range = 10000.f;
	Damage = 35.f;

	MaxAmmo = 17;
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();
	
	if (KannaCharacter)
	{
		KannaCharacter->AddWeaponToList(this); //권총은 시작할 때부터 가지고 있어야 한다.

		this->AttachToComponent(KannaCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Pistol_Socket"));
	}

	TotalAmmo = 255;
	CurrentAmmo = 17;

	GunMesh->SetVisibility(false);
	OnVisibilityChanged(false);
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
					UE_LOG(LogTemp, Warning, TEXT("EX 스킬!!"));
					//투사체 스폰
					FVector SpawnLocation = Muzzle->GetComponentLocation();
					FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, HitResult.ImpactPoint);
					
					FActorSpawnParameters Param;
					Param.Instigator = GetInstigator();
					GetWorld()->SpawnActor<AProjectile>(ExProjectileClass, SpawnLocation, SpawnRotation, Param);
				}
				else
				{
					if (IHitInterface* HitObject = Cast<IHitInterface>(HitResult.GetActor()))
					{
						HitObject->GetHit();
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

	//총구 화염 효과
	PlayMuzzleFlashEffect();


	//소음 효과
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.f,
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0),
		6000.f
	);
}

