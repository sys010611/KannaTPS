// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ExProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AExProjectile::AExProjectile()
{
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	TextOffset = 80.f;
}

// Called when the game starts or when spawned
void AExProjectile::BeginPlay()
{
	Super::BeginPlay();

	Projectile->OnComponentHit.AddDynamic(this, &AExProjectile::Damage);
}

// Called every frame
void AExProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExProjectile::Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 命中したアクターが AEnemy の場合
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		if (!Enemy->IsDead())
		{
			UGameplayStatics::ApplyDamage(OtherActor, 100.f, GetInstigator()->GetController(), this, UDamageType::StaticClass());
			SpawnBustedText(OtherActor->GetActorLocation() + FVector::UpVector * TextOffset, GetInstigator()->GetActorLocation());

			if (ExHitCameraShake)
				UGameplayStatics::PlayWorldCameraShake(GetWorld(), ExHitCameraShake, GetActorLocation(), 0.f, 100000.f);

			UGameplayStatics::PlaySound2D(GetWorld(), HitSound);

			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);

			UWorld* World = GetWorld();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer
			(TimerHandle, [World]() { UGameplayStatics::SetGlobalTimeDilation(World, 1.f); }, 0.3f, false);
		}
	}
	Destroy();
}

void AExProjectile::SpawnBustedText(const FVector& SpawnLocation, const FVector& InstigatorLocation)
{
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, InstigatorLocation);
	SpawnRotation.Pitch = 0.f;
	SpawnRotation += FRotator(0.f, 180.f, 0.f); // 反転させる

	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(BustedText, SpawnLocation, SpawnRotation, Param);
}

