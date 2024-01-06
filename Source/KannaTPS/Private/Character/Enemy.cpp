// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Weapons/AssultRifle.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/Controller.h"

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

	//엉뚱한 곳을 향하지 않도록
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
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

bool AEnemy::IsDead()
{
	return Attributes->IsDead();
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
	//죽을 때 래그돌 효과 (함수가 너무 길어져서 따로 떼어냄)
	RagdollEffect(DamageEvent);

	//AI 컨트롤러 떼기
	GetController()->UnPossess();

	CeaseFire();
}

void AEnemy::RagdollEffect(const FDamageEvent& DamageEvent)
{
	// 캡슐 콜라이더 비활성화
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true); // 래그돌을 활성화하는 핵심 함수이다.
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

void AEnemy::NoticePlayer()
{
	//이동속도 증가
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AEnemy::AwarePlayer()
{
	//이동속도 증가
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
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

