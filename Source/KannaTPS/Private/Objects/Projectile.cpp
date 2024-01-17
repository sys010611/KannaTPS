// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/KannaCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Projectile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile"));
	SetRootComponent(Projectile);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 6000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Projectile->OnComponentHit.AddDynamic(this, &AProjectile::Damage);

	SetLifeSpan(2.f);
}

void AProjectile::Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//맞은 액터가 칸나였을 경우
	if (Cast<AKannaCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어 피격!"));
		UGameplayStatics::ApplyDamage(OtherActor, 20.f, GetInstigator()->GetController(), this, UDamageType::StaticClass());
	}

	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

