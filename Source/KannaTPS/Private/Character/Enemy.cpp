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

	// 変な方向を向かないように
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
		if (AssultRifle)
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

	// プレイヤーの方を見るように
	FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetCharacter->GetActorLocation());

	// ピッチの設定
	Pitch = DesiredRotation.Pitch;
	if (TargetCharacter->bIsCrouched)
		Pitch += 5;

	// その他の設定
	SetActorRotation(FRotator(0.f, DesiredRotation.Yaw, DesiredRotation.Roll));

	// 発射体をスポーン
	FVector SpawnLocation = BulletStartPos->GetComponentLocation();
	FRotator SpawnRotation = BulletStartPos->GetComponentRotation();
	// 発射体の方向にランダム性を付与
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


		// 弾丸の衝撃を受ける演出
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			FPointDamageEvent PointDmg = *((FPointDamageEvent*)(&DamageEvent));
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit Point: %s"), *(PointDmg.HitInfo.BoneName.ToString()));
				if (PointDmg.HitInfo.BoneName == FName("Neck")) // ヘッドショットなら追加ダメージ
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

		// プレイヤー感知刺激
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
	// 死亡時のラグドール効果（関数が長くなりすぎたので分離）
	RagdollEffect(DamageEvent);

	// AIコントローラーを外す
	GetController()->UnPossess();

	CeaseFire();

	// 自分自身を敵リストから削除
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
	// カプセルコライダーを無効化
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true); // ラグドールを有効化する核心関数
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	// Movement Component を無効化
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComp)
	{
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();
		MovementComp->SetComponentTickEnabled(false);
	}

	// 30秒後に死体が消える
	SetLifeSpan(30.f);
	AssultRifle->SetLifeSpan(30.f);

	// 弾丸の衝撃を受ける演出
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
	// 移動速度を上昇

	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	// Alert状態でない場合 GameManager を通して Alert状態に変更
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
	// 移動速度を上昇
	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit() // パーティクルエフェクトの処理担当、ダメージ判定は別の TakeDamage で行う
{
	PlayHitMontage();
}

