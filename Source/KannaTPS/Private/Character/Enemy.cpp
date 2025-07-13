// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "Weapons/Gun.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/Controller.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Touch.h"
#include "Objects/Projectile.h"
#include "Managers/GameManager.h"
#include "Managers/ConversationManager.h"
#include "Engine/DamageEvents.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetGenerateOverlapEvents(true);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	Attributes->SetComponentTickEnabled(false);
	//HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	//HealthBarWidget->SetupAttachment(GetRootComponent());

	//엉뚱한 곳을 향하지 않도록
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	BulletStartPos = CreateDefaultSubobject<UArrowComponent>(TEXT("BulletStartPos"));
	BulletStartPos->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (AssultRifleClass)
	{
		FActorSpawnParameters Param;
		Param.Owner = this;
		AssultRifle = GetWorld()->SpawnActor<AGun>(AssultRifleClass, Param);
		if(AssultRifle)
			AssultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Rifle_Socket"));
	}


	if (GetGameInstance()->GetSubsystem<UGameManager>()->IsAlerted)
	{
		NoticePlayer();
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::OneShot()
{
	if (!TargetCharacter)
		return;

	// 플레이어를 바라보도록
	FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetCharacter->GetActorLocation());

	//피치 설정
	Pitch = DesiredRotation.Pitch;
	if (TargetCharacter->bIsCrouched)
		Pitch += 5;

	//그 외 설정
	SetActorRotation(FRotator(0.f, DesiredRotation.Yaw, DesiredRotation.Roll));

	//투사체 스폰
	FVector SpawnLocation = BulletStartPos->GetComponentLocation();
	FRotator SpawnRotation = BulletStartPos->GetComponentRotation();
	//투사체 방향에 랜덤성 부여
	SpawnRotation.Pitch += FMath::RandRange(-3.f, 3.f);
	SpawnRotation.Yaw += FMath::RandRange(-3.f, 3.f);

	FActorSpawnParameters Param;
	Param.Instigator = this;
	GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, Param);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GunSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);
	AssultRifle->PlayMuzzleFlashEffect();
	
	return;
}

void AEnemy::Shoot()
{
	GetWorldTimerManager().SetTimer(ShootTimer, this, &AEnemy::OneShot, .1f, true);
}

bool AEnemy::IsDead()
{
	return Attributes->IsDead();
}

bool AEnemy::IsNotEngaged()
{
	return TargetCharacter == nullptr;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		if (Attributes->IsDead())
			return DamageAmount;


		// 총알의 충격을 받는 연출
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			FPointDamageEvent PointDmg = *((FPointDamageEvent*)(&DamageEvent));
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit Point: %s"), *(PointDmg.HitInfo.BoneName.ToString()));
				if (PointDmg.HitInfo.BoneName == FName("Neck")) //헤드샷 맞으면 추가 데미지
				{
					DamageAmount += 25.f;
				}
			}
		}

		Attributes->ReceiveDamage(DamageAmount);
		
		if (Attributes->IsDead())
			Die(DamageEvent);
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy HP: %f"), Attributes->GetCurrentHealth())

	// 플레이어 감지 자극
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(this);
	PerceptionSystem->OnEvent(FAITouchEvent(this, EventInstigator->GetPawn(), GetActorLocation()));

	if (BulletHitSound)
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);

	return DamageAmount;
}

void AEnemy::PlayHitMontage()
{
	if (HitMontageSections.Num() <= 0) return;
	const int32 MaxIndex = HitMontageSections.Num() - 1;
	const int32 Index = FMath::RandRange(0, MaxIndex);

	if (HitMontage)
	{
		PlayMontageBySection(HitMontage, HitMontageSections[Index]);
	}
}

void AEnemy::PlayMontageBySection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
		//UE_LOG(LogTemp,Warning, TEXT("적 피격 애니메이션"));
	}
}

void AEnemy::Die(FDamageEvent const& DamageEvent)
{
	//죽을 때 래그돌 효과 (함수가 너무 길어져서 따로 떼어냄)
	RagdollEffect(DamageEvent);

	//AI 컨트롤러 떼기
	GetController()->UnPossess();

	CeaseFire();

	// 자기 자신을 적 리스트에서 삭제
	if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
	{
		GM->RemainingEnemyCount[GM->CurrentFloor]--;
		UE_LOG(LogTemp, Warning, TEXT("%d floor REMAINING : %d"), GM->CurrentFloor, GM->RemainingEnemyCount[GM->CurrentFloor]);

		if (GM->CheckIfCleared())
		{
			GetGameInstance()->GetSubsystem<UConversationManager>()->SetMessage(TEXT("현재 층 확보 완료"));
		}
	}
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
	AssultRifle->SetLifeSpan(30.f);

	// 총알의 충격을 받는 연출
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent PointDmg = *((FPointDamageEvent*)(&DamageEvent));
		{
			GetMesh()->AddImpulseAtLocation(PointDmg.ShotDirection * 10000, PointDmg.HitInfo.ImpactPoint, PointDmg.HitInfo.BoneName);
		}
	}
}

void AEnemy::NoticePlayer()
{
	TargetCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	SetBBTargetActor();
	//이동속도 증가

	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	// Alert상태가 아니었을 경우 GameManager를 통해 Alert상태로 전환
	if (UGameManager* GM = GetWorld()->GetGameInstance()->GetSubsystem<UGameManager>())
	{
		if (GM->IsAlerted == false)
		{
			GM->Alert();
		}
	}
}

void AEnemy::AwarePlayer()
{
	//이동속도 증가
	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit() // 파티클 이펙트 기능 담당, 데미지 판정은 따로 TakeDamage에서
{
	PlayHitMontage();
}

