// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KannaCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Animation/AnimMontage.h"
#include "Weapons/Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "HUD/KannaTPSHUD.h"
#include "HUD/KannaTPSOverlay.h"
#include "Components/AttributeComponent.h"
#include "HUD/DamageIndicator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Objects/Halo.h"
#include "Managers/GameManager.h"
#include "Managers/ConversationManager.h"
#include "Components/AudioComponent.h"
#include "HUD/KannaTPSOption.h"

// Sets default values
AKannaCharacter::AKannaCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmDefaultLength = 200.f;
	SpringArmDefaultOffset = FVector(0.f, 50.f, 20.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = SpringArmDefaultLength;
	SpringArm->SocketOffset = SpringArmDefaultOffset;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(SpringArm);

	// enable crouching
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// 近接攻撃ヒットボックスのセットアップ
	PunchHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("PunchHitbox"));
	KickHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("KickHitbox"));
	// ソケットにアタッチ
	PunchHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("LeftHand"));
	KickHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightFoot"));
	// デフォルトでは当たり判定なし
	PunchHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KickHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

// Called when the game starts or when spawned
void AKannaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	PunchHitbox->OnComponentBeginOverlap.AddDynamic(this, &AKannaCharacter::OnHitboxOverlap);
	KickHitbox->OnComponentBeginOverlap.AddDynamic(this, &AKannaCharacter::OnHitboxOverlap);

	AimingDirection = EAimingDirection::EAD_Neutral;
	IsCameraMoving = false;

	// ウィジェットの初期化
	InitKannaTpsOverlay();

	// 画面ダメージ用ポストプロセスを生成・初期化
	ScreenDamageDynamic = UMaterialInstanceDynamic::Create(ScreenDamage, GetWorld());
	APostProcessVolume* Volume = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	if (Volume)
		Volume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.f, ScreenDamageDynamic));

	// ハローと拳銃をスポーン
	Halo = GetWorld()->SpawnActor<AHalo>(HaloClass);
	GetWorld()->SpawnActor(PistolClass);

	// FLatentInfo の初期化

	FadeInfo.CallbackTarget = this;
	FadeInfo.Linkage = 0;
	FadeInfo.ExecutionFunction = FName("FadeOutDamageIndicator");
	FadeInfo.UUID = 1;

	HealthInfo.CallbackTarget = this;
	HealthInfo.Linkage = 0;
	HealthInfo.ExecutionFunction = FName("EnableHealthRegen");
	HealthInfo.UUID = 2;

	if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
	{
		GM->KannaCharacter = this;
		GM->IsAlerted = false;
		MouseSensitivity = GM->MouseSensitivity;
		GM->StartTime = FDateTime::Now();
	}

	SetCanBeDamaged(false);
}

// Called every frame
void AKannaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 静止してカバー中のときのキャラクター向きを決定
	if (IsInCover && (GetVelocity().Length() == 0) && ActionState == EActionState::EAS_Neutral)
	{
		FVector Forward = GetCharacterMovement()->GetPlaneConstraintNormal() * -1.f; // 壁面を向く方向

		if (bIsCrouched)
		{
			AddMovementInput(Forward);
		}
		else
		{
			if (IsCameraAtRight()) // カメラが右側 → キャラクターも右向き
			{
				FRotator RightRotator = Forward.Rotation() + UKismetMathLibrary::MakeRotator(0, 0, 90.f);
				SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), RightRotator, DeltaTime, 1000.f));
			}
			else // カメラが左側 → キャラクターも左向き
			{
				FRotator LeftRotator = Forward.Rotation() + UKismetMathLibrary::MakeRotator(0, 0, -90.f);
				SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), LeftRotator, DeltaTime, 1000.f));
			}

		}
	}

	// 画面端のダメージエフェクト
	float ScreenDamageRadius =
		FMath::GetMappedRangeValueClamped(TRange<float>(0.f, 100.f), TRange<float>(0.3f, 1.f), Attributes->GetCurrentHealth());
	ScreenDamageDynamic->SetScalarParameterValue(FName("Radius"), ScreenDamageRadius);

	// EXゲージと同期
	if (Attributes && KannaTPSOverlay)
		KannaTPSOverlay->SetExGaugePercent(Attributes->GetExGaugePercent());
}

void AKannaCharacter::InitKannaTpsOverlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		AKannaTPSHUD* KannaTPSHUD = Cast<AKannaTPSHUD>(PlayerController->GetHUD());
		if (KannaTPSHUD)
		{
			KannaTPSOverlay = KannaTPSHUD->GetKannaTPSOverlay();
			if (KannaTPSOverlay)
			{
				KannaTPSOverlay->HideAmmoText();
			}
		}
		DamageIndicator = CreateWidget<UDamageIndicator>(PlayerController, DamageIndicatorClass);
		DamageIndicator->AddToViewport();
		DamageIndicator->SetRenderOpacity(0.f);
	}
}

float AKannaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && CanBeDamaged())
	{
		Attributes->ReceiveDamage(DamageAmount);

		if (Attributes->IsDead())
		{
			Die();
		}
		else if (Attributes->GetCurrentHealth() < 40.f)
		{
			bool IsStunned = UKismetMathLibrary::RandomBoolWithWeight(0.4f);
			if (IsStunned)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance && StunMontage)
				{
					DisableMovement();

					// タイマーを使って 1 秒後にスタン解除
					FTimerHandle StunTimerHandle;
					GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AKannaCharacter::EnableMovement, 1.f);

					if (CanBeStunned())
					{
						AnimInstance->Montage_Play(StunMontage);
						ActionState = EActionState::EAS_Stunned;
					}
				}
			}
		}
		else
		{
			if (ActionState != EActionState::EAS_Rolling)
				PlayHitMontage();
		}

		if (Attributes->GetCurrentHealth() < 30.f)
		{
			GetGameInstance()->GetSubsystem<UConversationManager>()->SetMessage(TEXT("헤일로가 깨질 것 같아.."));
		}

		// ダメージを受けてから 3 秒後に自動回復開始
		Attributes->DisableHealthRegen();

		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), 3.f, HealthInfo);
	}

	if (DamageIndicator && EventInstigator)
	{
		DamageIndicator->SetRenderOpacity(1.f);

		DamageIndicator->Causer = EventInstigator->GetPawn();

		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), 3.f, FadeInfo);
	}

	if (BulletHitSound)
		UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);

	if (HitCameraShake)
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), HitCameraShake, GetActorLocation(), 0.f, 500.f);

	return DamageAmount;
}

bool AKannaCharacter::CanBeStunned()
{
	return ActionState != EActionState::EAS_Stunned
		&& ActionState != EActionState::EAS_Rolling
		&& ActionState != EActionState::EAS_Reloading;
}

void AKannaCharacter::FadeOutDamageIndicator()
{
	DamageIndicator->PlayFadeAnim();
}

void AKannaCharacter::Die()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	ZoomOutCamera();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DieMontage)
	{
		PlayDieMontage(AnimInstance);
	}

	Halo->BreakHalo();

	SpringArm->SocketOffset = FVector::UpVector * 30.f;
	SpringArm->TargetArmLength = 100.f;

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() { UGameplayStatics::OpenLevel(GetWorld(), FName("GameOver")); },
		2.f, false);
}

void AKannaCharacter::EnableMovement()
{
	Controller->SetIgnoreMoveInput(false);
	ActionState = EActionState::EAS_Neutral;
}

void AKannaCharacter::EnableHealthRegen()
{
	Attributes->EnableHealthRegen();
}

void AKannaCharacter::SetPunchHitbox(ECollisionEnabled::Type CollisionEnabled)
{
	if (PunchHitbox) // null チェック
		PunchHitbox->SetCollisionEnabled(CollisionEnabled);
}

void AKannaCharacter::SetKickHitbox(ECollisionEnabled::Type CollisionEnabled)
{
	if (KickHitbox) // null チェック
		KickHitbox->SetCollisionEnabled(CollisionEnabled);
}

void AKannaCharacter::Move(const FInputActionValue& Value)
{
	if (IsInCover) // カバー中の移動処理は CoverTrace 関数で行う
	{
		CoverTrace();
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AKannaCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity);
	AddControllerYawInput(LookAxisVector.X * MouseSensitivity);
}

void AKannaCharacter::Aim()
{
	//OnAimStart(); // ブループリントイベント（カメラズーム）

	if (CharacterState == ECharacterState::ECS_Unarmed || ActionState != EActionState::EAS_Neutral) return;

	if (IsInCover)
	{
		if (!GetCharacterMovement()->IsCrouching()) // 立ち姿勢でカバー中 → 右狙い・左狙いに分岐
		{
			FHitResult HitResult;

			FCollisionQueryParams CollisionParameters;
			CollisionParameters.AddIgnoredActor(this);

			FVector ActorLocation = GetActorLocation();
			UCharacterMovementComponent* Movement = GetCharacterMovement();

			// 壁の方向
			FVector WallDirection = Movement->GetPlaneConstraintNormal() * (-1.f);

			CheckLeftRightHit(WallDirection, ActorLocation, HitResult, CollisionParameters);

			if (LeftHit && !RightHit)
				AimingDirection = EAimingDirection::EAD_Right;
			else if (RightHit && !LeftHit)
				AimingDirection = EAimingDirection::EAD_Left;
			else if (!RightHit && !LeftHit)
			{
				if (IsCameraAtRight())
					AimingDirection = EAimingDirection::EAD_Right;
				else
					AimingDirection = EAimingDirection::EAD_Left;
			}

			else if (RightHit && LeftHit)// 右・左どちらにも空きがない → エイム不可
			{
				return;
			}
		}
		else
		{
			AimingDirection = EAimingDirection::EAD_Neutral;
		}
	}
	else
	{
		AimingDirection = EAimingDirection::EAD_Neutral;
	}

	ActionState = EActionState::EAS_Aiming;

	GetCharacterMovement()->MaxWalkSpeed = 200.f; // エイム中は移動速度を低下
	GetCharacterMovement()->bOrientRotationToMovement = false; // キャラクターは移動入力で回転しない（エイム方向を維持）
}

void AKannaCharacter::ReleaseAim()
{
	if (ActionState != EActionState::EAS_Reloading) // リロード中はエイムを解いてもリロード継続
	{
		ActionState = EActionState::EAS_Neutral;
	}

	if (!IsInCover)
		GetCharacterMovement()->MaxWalkSpeed = 400.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AKannaCharacter::SwitchWeapon()
{
	if (ActionState != EActionState::EAS_Neutral) return;

	if (CharacterState == ECharacterState::ECS_ArmedWithPistol) // 拳銃 → 素手
	{
		CharacterState = ECharacterState::ECS_Unarmed;
		CurrentWeapon->GetMesh()->SetVisibility(false);
		CurrentWeapon->OnVisibilityChanged(false);
		CurrentWeapon = nullptr;
	}
	else if (CharacterState == ECharacterState::ECS_Unarmed) // 素手 → 拳銃
	{
		if (WeaponList.Num() > 0)
		{
			CharacterState = ECharacterState::ECS_ArmedWithPistol;
			CurrentWeapon = WeaponList[0];
		}
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->SetInstigator(this);

		CurrentWeapon->GetMesh()->SetVisibility(true);
		CurrentWeapon->OnVisibilityChanged(true);
	}

	if (KannaTPSOverlay)
	{
		if (CurrentWeapon)
		{
			KannaTPSOverlay->ShowAmmoText();
			KannaTPSOverlay->SetTotalAmmoText(CurrentWeapon->GetTotalAmmo());
			KannaTPSOverlay->SetCurrentAmmoText(CurrentWeapon->GetCurrentAmmo());
		}
		else
		{
			KannaTPSOverlay->HideAmmoText();
		}
	}
}

void AKannaCharacter::Attack()
{
	if (ActionState != EActionState::EAS_Neutral || CharacterState == ECharacterState::ECS_Unarmed) return;

	//GetCharacterMovement()->DisableMovement(); // 攻撃中は移動できないようにする
	Controller->SetIgnoreMoveInput(true);

	PlayAttackMontage();
	ActionState = EActionState::EAS_Attacking;
}

void AKannaCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Neutral;
	//GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking; // 攻撃終了後に MovementMode を初期化
	Controller->SetIgnoreMoveInput(false);
}

void AKannaCharacter::Roll()
{
	if (ActionState != EActionState::EAS_Neutral || IsInCover) return; // 回避はニュートラル状態のときのみ可能

	PlayRollMontage();
	ActionState = EActionState::EAS_Rolling;

	OnRollStart(); // カメラワーク用イベントを呼び出す

	// カプセルコライダーのサイズを半分にする
	Crouch();
}

void AKannaCharacter::PlayMontageBySection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void AKannaCharacter::PlayAttackMontage()
{
	if (AttackMontageSections.Num() <= 0) return;
	const int32 MaxIndex = AttackMontageSections.Num() - 1;
	const int32 Index = FMath::RandRange(0, MaxIndex);

	if (AttackMontage)
	{
		PlayMontageBySection(AttackMontage, AttackMontageSections[Index]);
	}
}

void AKannaCharacter::PlayRollMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && RollMontage)
	{
		AnimInstance->Montage_Play(RollMontage);
	}
}

void AKannaCharacter::PlayHitMontage()
{
	if (HitMontageSections.Num() <= 0) return;
	const int32 MaxIndex = HitMontageSections.Num() - 1;
	const int32 Index = FMath::RandRange(0, MaxIndex);

	if (HitMontage)
	{
		PlayMontageBySection(HitMontage, HitMontageSections[Index]);
	}
}

void AKannaCharacter::PlayDieMontage(UAnimInstance* AnimInstance)
{
	AnimInstance->Montage_Play(DieMontage);
}

bool AKannaCharacter::CanUseExSkill()
{
	return Attributes->GetExGaugePercent() >= 0.3f;
}

bool AKannaCharacter::IsCameraAtRight()
{
	return SpringArm->SocketOffset.Y > 0;
}

void AKannaCharacter::RollEnd()
{
	ActionState = EActionState::EAS_Neutral;

	// カプセルコライダーのサイズを元に戻す
	if (!IsInCover)
		UnCrouch();
}

void AKannaCharacter::OnHitboxOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("%s"), *(OverlappedComponent->GetName()));

	UGameplayStatics::ApplyDamage(
		OtherActor,
		100.f,
		GetController(),
		this,
		UDamageType::StaticClass()
	);

	if (GetWorld())
		UGameplayStatics::PlaySound2D(GetWorld(), MeleeAttackSound);
}

void AKannaCharacter::Fire() // ここでは状態遷移とアニメーションの再生のみ行う
{
	if (ActionState != EActionState::EAS_Aiming) return; // エイム中のみ射撃可能
	if (CurrentWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	FVector StartPoint = ViewCamera->GetComponentLocation() + ViewCamera->GetForwardVector() * 100;
	FVector Direction = ViewCamera->GetForwardVector();

	if (CurrentWeapon && CurrentWeapon->IsShootable()) // null チェック → 発射可能か確認
	{
		if (FireCameraShake)
		{
			UGameplayStatics::PlayWorldCameraShake(GetWorld(), FireCameraShake, GetActorLocation(), 0.f, 500.f);
		}

		if (CurrentWeapon->IsExSkillReady()) // Ex 射撃
		{
			if (ExSkillGunSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ExSkillGunSound);
			}
			if (AnimInstance && ExFireMontage)
			{
				AnimInstance->Montage_Play(ExFireMontage);
			}
		}
		else // 通常射撃
		{
			if (GunSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), GunSound);
			}
			if (AnimInstance && FireMontage)
			{
				AnimInstance->Montage_Play(FireMontage);
			}
		}

		CurrentWeapon->Fire(StartPoint, Direction); // 銃の発射処理をインターフェイスにデリゲート
		SpreadCrosshair();
	}
}

void AKannaCharacter::Reload()
{
	if (CurrentWeapon && CurrentWeapon->IsReloadable())
	{
		ReleaseAim();
		ActionState = EActionState::EAS_Reloading;
	}
}

void AKannaCharacter::ReloadEnd()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload(); // 銃のリロード処理をインターフェイスに委譲
		ActionState = EActionState::EAS_Neutral;
	}
}

void AKannaCharacter::TakeCover()
{
	if (IsInCover)
	{
		StopCover();
	}
	else
	{
		WallTrace(); // カバーできる壁があるか検知
	}
}

void AKannaCharacter::ExSkill()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->HasExSkill && !CurrentWeapon->IsExSkillReady() && CanUseExSkill() && CurrentWeapon->IsShootable())
		{
			CurrentWeapon->ReadyExSkill(); // 武器側にデリゲートする
			Attributes->SubtractExGaugePercent(0.3f);
		}
	}
}

void AKannaCharacter::WallTrace()
{
	FVector ActorLocation = GetActorLocation();
	if (GetCharacterMovement()->IsCrouching())
	{
		ActorLocation += FVector(0.f, 0.f, 44.f);
	}

	// 低いカバーを検知
	FVector LowStart = GetActorLocation();
	FVector LowEnd = GetActorLocation() + GetActorForwardVector() * 100.f;
	// 高いカバーを検知
	FVector HighStart = GetActorLocation() + FVector(0.f, 0.f, 70.f);
	FVector HighEnd = HighStart + GetActorForwardVector() * 100.f;

	//DrawDebugLine(GetWorld(), HighStart, HighEnd, FColor(255, 0, 0), true, 10.f, 0, 5.f);
	//DrawDebugLine(GetWorld(), LowStart, LowEnd, FColor(255, 0, 0), true, 10.f, 0, 5.f);

	if (GetWorld())
	{
		FHitResult HitResult;

		FCollisionQueryParams CollisionParameters; // トレースで自分自身は無視するようにする
		CollisionParameters.AddIgnoredActor(this);

		// 高いカバーの検知を試みる
		if (GetWorld()->LineTraceSingleByChannel(HitResult, HighStart, HighEnd, ECC_GameTraceChannel2, CollisionParameters))
		{
			StartCover(HitResult.Normal, false);
		}
		// 低いカバーの検知を試みる
		else if (GetWorld()->LineTraceSingleByChannel(HitResult, LowStart, LowEnd, ECC_GameTraceChannel2, CollisionParameters))
		{
			StartCover(HitResult.Normal, true);
		}
	}
}

void AKannaCharacter::CoverTrace()
{
	FHitResult HitResult;

	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(this);

	FVector ActorLocation = GetActorLocation();
	UCharacterMovementComponent* Movement = GetCharacterMovement();

	// 現在の PlaneConstraintNormal はプレイヤーの移動制限平面の法線ベクトルであり、壁の外側方向を向いている。
	// キャラクターが壁の方を向くベクトルが欲しいので、-1 を掛けて反転させる。
	FVector WallDirection = Movement->GetPlaneConstraintNormal() * (-1.f);

	CheckLeftRightHit(WallDirection, ActorLocation, HitResult, CollisionParameters);

	FVector2D MoveVector = MoveActionBinding->GetValue().Get<FVector2D>(); // MoveActionBinding は移動入力値を参照するために用意したもの。
	// PlayerRightVector はカメラ視点基準の右方向ベクトル。
	FVector PlayerRightVector = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotator(0.f, 0.f, GetControlRotation().Yaw));

	// 左右どちらにもカバー用スペースが残っていれば、どちらにも移動できる。
	if (LeftHit && RightHit)
	{
		if (MoveVector.X != 0.f) // 左右入力が 0 でない場合
		{
			// 前方にカバー物があるかをライン・トレースで再確認し、平面制限を掛け直したうえで AddMovementInput で移動させる。
			// 再度制限を掛けるのは、円柱形のカバーでも自然に左右移動できるようにするため。
			if (GetWorld()->LineTraceSingleByChannel(
				HitResult,
				ActorLocation,
				ActorLocation + WallDirection * 200.f,
				ECollisionChannel::ECC_GameTraceChannel2,
				CollisionParameters)
				)
			{
				Movement->SetPlaneConstraintNormal(HitResult.Normal);

				AddMovementInput(PlayerRightVector, MoveVector.X);
			}

		}
	}
	else // 左右どちらか一方にしかカバー用スペースが残っていない場合
	{
		float MovementScale; // 必要に応じて 0 になることもある。

		// 右側にスペースがあり、右入力が来た場合はスケールをそのまま使う。
		if (RightHit && MoveVector.X > 0)
		{
			MovementScale = MoveVector.X;
		}
		// 左側も同様。
		else if (LeftHit && MoveVector.X < 0)
		{
			MovementScale = MoveVector.X;
		}
		// 残っているスペースの方向と入力方向が反対のときは MovementScale を 0 にして移動させない。
		else
		{
			MovementScale = 0;
		}
		// 計算された MovementScale に応じてキャラクターを移動させる。
		AddMovementInput(PlayerRightVector, MovementScale);
	}

	bool bIsCameraAtRight = IsCameraAtRight();
	if ((MoveVector.X > 0 && !bIsCameraAtRight) || (MoveVector.X < 0 && bIsCameraAtRight))
	{
		SwitchCameraPos();
	}
}

void AKannaCharacter::CheckLeftRightHit(FVector& WallDirection, FVector& ActorLocation, FHitResult& HitResult, FCollisionQueryParams& CollisionParameters)
{
	// RightVector は壁を向いたときの右方向を表す。まず Rotator を作り、そこからベクトルを取り出す。
	// 変数名は RightRotator だが、RightVector を取り出すためのものであり、実際の向きは WallDirection である。
	// MakeRotFromX は X 軸ベクトルから Rotator を作る関数という意味。
	FRotator RightRotator = UKismetMathLibrary::MakeRotFromX(WallDirection);
	FVector RightVector = UKismetMathLibrary::GetRightVector(RightRotator);

	// キャラクターの少し右側から壁方向へライン・トレースを行う。
	FVector RightStart = ActorLocation + RightVector * 45.f;
	FVector RightEnd = RightStart + WallDirection * 200.f;

	// ライントレースがヒットした場合 RightHit が true になる。
	RightHit = GetWorld()->LineTraceSingleByChannel
	(HitResult,
		RightStart,
		RightEnd,
		ECollisionChannel::ECC_GameTraceChannel2,
		CollisionParameters);

	// 今度は同じ処理を左側で行う。
	// WallDirection の反対方向から Rotator を作り、そこから RightVector を取り出すとキャラクター基準の左方向になる。
	FRotator LeftRotator = UKismetMathLibrary::MakeRotFromX(WallDirection * (-1.f));
	FVector LeftVector = UKismetMathLibrary::GetRightVector(LeftRotator);

	FVector LeftStart = ActorLocation + LeftVector * 45.f;
	FVector LeftEnd = LeftStart + WallDirection * 200.f;

	// LeftHit を設定する
	LeftHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		LeftStart,
		LeftEnd,
		ECollisionChannel::ECC_GameTraceChannel2,
		CollisionParameters);
}

void AKannaCharacter::StartCover(FVector& PlaneNormal, bool IsLowCover)
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	GetCharacterMovement()->SetPlaneConstraintNormal(PlaneNormal);

	IsInCover = true;

	if (IsLowCover)
	{
		Crouch();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
}

void AKannaCharacter::StopCover()
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(false);

	IsInCover = false;

	UnCrouch();

	if (ActionState != EActionState::EAS_Aiming)
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	//GetCharacterMovement()->RotationRate.Yaw = 500.f;
}

void AKannaCharacter::SetDeadScreen()
{
	KannaTPSOverlay->SetDeadScreen();
	DamageIndicator->SetVisibility(ESlateVisibility::Hidden);

	// ミュートする
	if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
		GM->Mute();
}

void AKannaCharacter::PlayBGM()
{
	if (BGM)
	{
		BGM_ref = UGameplayStatics::SpawnSound2D(GetWorld(), BGM);
	}
}

void AKannaCharacter::StopBGM()
{
	BGM_ref->SetActive(false);
}

// Called to bind functionality to input
void AKannaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Move);
		MoveActionBinding = &(EnhancedInputComponent->BindActionValue(MoveAction));
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Look);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AKannaCharacter::ReleaseAim);
		//EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Interact);
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AKannaCharacter::SwitchWeapon);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Attack);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Roll);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Fire);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Reload);
		EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Triggered, this, &AKannaCharacter::TakeCover);
		EnhancedInputComponent->BindAction(SwitchCameraAction, ETriggerEvent::Triggered, this, &AKannaCharacter::SwitchCameraPos);
		EnhancedInputComponent->BindAction(ExSkillAction, ETriggerEvent::Triggered, this, &AKannaCharacter::ExSkill);
	}
}
