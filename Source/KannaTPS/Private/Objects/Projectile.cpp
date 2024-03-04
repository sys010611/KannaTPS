// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/KannaCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	ProjectileMovement->InitialSpeed = 10000.f;
	ProjectileMovement->MaxSpeed = 10000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Projectile->OnComponentHit.AddDynamic(this, &AProjectile::Damage);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectile::Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//맞은 액터가 칸나였을 경우
	if (Cast<AKannaCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 10.f, GetInstigator()->GetController(), this, UDamageType::StaticClass());
	}

	Destroy();
}
