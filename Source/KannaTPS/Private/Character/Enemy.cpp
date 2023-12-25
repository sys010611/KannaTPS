// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetGenerateOverlapEvents(true);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	//HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	//HealthBarWidget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//if (HealthBarWidget)
	//{
	//	HealthBarWidget->SetHealthPercent(.7f);
	//}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
		
		if (Attributes->IsDead())
			Die(DamageEvent);
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy HP: %f"), Attributes->GetCurrentHealth())

	return DamageAmount;
}

void AEnemy::Die(FDamageEvent const& DamageEvent)
{
	// 캡슐 콜라이더 비활성화
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true); // 래그돌을 활성화하는 핵심 함수이다. 사실 이것 하나만 있어도 되긴 한다.
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	//Movement Component 비활성화
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComp)
	{
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();
		MovementComp->SetComponentTickEnabled(false);
	}

	//30초 뒤에 시체 사라짐
	SetLifeSpan(30.f);

	// 총알의 충격을 받는 연출
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent PointDmg = *((FPointDamageEvent*)(&DamageEvent));
		{
			GetMesh()->AddImpulseAtLocation(PointDmg.ShotDirection * 10000, PointDmg.HitInfo.ImpactPoint, PointDmg.HitInfo.BoneName);
			UE_LOG(LogTemp, Warning, TEXT("Hit Point: %s"), *(PointDmg.HitInfo.BoneName.ToString()));
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit() // 파티클 이펙트 기능 담당, 데미지 판정은 따로 TakeDamage에서
{
	
}

