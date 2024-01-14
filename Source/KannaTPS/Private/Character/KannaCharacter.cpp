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

// Sets default values
AKannaCharacter::AKannaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmDefaultLength=200.f;
	SpringArmDefaultOffset=FVector(0.f,50.f,20.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = SpringArmDefaultLength;
	SpringArm->SocketOffset = SpringArmDefaultOffset;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(SpringArm);

	// enable crouching
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	//근접공격 히트박스 셋업
	PunchHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("PunchHitbox"));
	KickHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("KickHitbox"));
	//소켓에 부착
	PunchHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("LeftHand"));
	KickHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("RightFoot"));
	//충돌판정 없음이 기본
	PunchHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KickHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	IsCameraAtRight = true;

	AimingDirection = EAimingDirection::EAD_Neutral;

	InitKannaTpsOverlay();
}

// Called every frame
void AKannaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 엄폐중이고, 키보드에서 손을 뗐을 시 캐릭터의 방향 결정
	if (IsInCover && (GetVelocity().Length() == 0))
	{
		if (bIsCrouched)
		{
			FVector Direction = GetCharacterMovement()->GetPlaneConstraintNormal() * -1.f; //벽면을 바라봄
			AddMovementInput(Direction);
		}
		//else
		//{
		//	FVector Direction = GetCharacterMovement()->GetPlaneConstraintNormal() * -1.f; //벽을 등짐
		//	AddMovementInput(Direction);
		//}
	}
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
	}
}

void AKannaCharacter::SetPunchHitbox(ECollisionEnabled::Type CollisionEnabled)
{
	if (PunchHitbox) //널 체크
		PunchHitbox->SetCollisionEnabled(CollisionEnabled);
}

void AKannaCharacter::SetKickHitbox(ECollisionEnabled::Type CollisionEnabled)
{
	if (KickHitbox) //널 체크
		KickHitbox->SetCollisionEnabled(CollisionEnabled);
}

void AKannaCharacter::Move(const FInputActionValue& Value)
{
	if (IsInCover) // 엄폐중일 경우 이동은 CoverTrace 함수에서 담당한다.
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

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AKannaCharacter::Aim()
{
	//OnAimStart(); // 블루프린트 이벤트 (카메라 확대)

	if (CharacterState == ECharacterState::ECS_Unarmed || ActionState != EActionState::EAS_Neutral) return;

	if (IsInCover && !GetCharacterMovement()->IsCrouching()) //서서 엄폐중 -> 오른쪽, 왼쪽 조준으로 나뉨
	{
		FHitResult HitResult;

		FCollisionQueryParams CollisionParameters;
		CollisionParameters.AddIgnoredActor(this);

		FVector ActorLocation = GetActorLocation();
		UCharacterMovementComponent* Movement = GetCharacterMovement();

		//벽의 방향
		FVector WallDirection = Movement->GetPlaneConstraintNormal() * (-1.f);

		CheckLeftRightHit(WallDirection, ActorLocation, HitResult, CollisionParameters);

		if (LeftHit && !RightHit)
			AimingDirection = EAimingDirection::EAD_Right;
		else if(RightHit && !LeftHit)
			AimingDirection = EAimingDirection::EAD_Left;
		else if (!RightHit && !LeftHit)
			AimingDirection = EAimingDirection::EAD_Right;
		else if (RightHit && LeftHit)// 오른쪽, 왼쪽 모두 빈 공간이 없음 -> 조준 불가
		{
			return;
		}
	}
	else
	{
		AimingDirection = EAimingDirection::EAD_Neutral;
	}

	ActionState = EActionState::EAS_Aiming;

	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 조준 중에는 이동속도 감소
	GetCharacterMovement()->bOrientRotationToMovement = false; // 캐릭터가 방향키에 따라 회전하지 않음 (조준 방향 유지)


}

void AKannaCharacter::ReleaseAim()
{
	if (ActionState != EActionState::EAS_Reloading) // 장전 중이었을 때는 조준을 풀어도 계속 장전하도록
	{
		ActionState = EActionState::EAS_Neutral;
	}

	if(!IsInCover)
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		//SetNeutralStateSpeed();

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AKannaCharacter::Interact()
{
	if (OverlappingItem)
	{
		OverlappingItem->Get();
	}
}

void AKannaCharacter::SwitchWeapon()
{
	if(ActionState != EActionState::EAS_Neutral) return;

	if (CharacterState == ECharacterState::ECS_ArmedWithPistol) // 권총 -> 맨손
	{
		CharacterState = ECharacterState::ECS_Unarmed;
		CurrentWeapon = nullptr;
	}
	else if (CharacterState == ECharacterState::ECS_Unarmed) //맨손 -> 권총
	{
		CharacterState = ECharacterState::ECS_ArmedWithPistol;
		CurrentWeapon = WeaponList[0];
	}

	if(CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->SetInstigator(this);
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

	//GetCharacterMovement()->DisableMovement(); //공격 중에는 움직이지 않도록
	Controller->SetIgnoreMoveInput(true);

	PlayAttackMontage();
	ActionState = EActionState::EAS_Attacking;
}

void AKannaCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage) //null check
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 RandNum = FMath::RandRange(1, 3);
		FName SectionName = FName();

		switch (RandNum)
		{
		case 1:
			SectionName = FName("Attack1");
			break;
		case 2:
			SectionName = FName("Attack2");
			break;
		case 3:
			SectionName = FName("Attack3");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void AKannaCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Neutral;
	//GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking; //공격 완료 후 Movement Mode 초기화 
	Controller->SetIgnoreMoveInput(false);
}

void AKannaCharacter::Roll()
{
	if(ActionState != EActionState::EAS_Neutral || IsInCover) return; //구르기는 중립 상태에서만 가능

	PlayRollMontage();
	ActionState = EActionState::EAS_Rolling;

	OnRollStart(); // 카메라 워킹 이벤트 Invoke

	//캡슐 콜라이더 크기 반으로 줄이기
	Crouch();
}

void AKannaCharacter::PlayRollMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && RollMontage)
	{
		AnimInstance->Montage_Play(RollMontage);
	}
}

void AKannaCharacter::RollEnd()
{
	ActionState = EActionState::EAS_Neutral;

	//캡슐 콜라이더 크기 원상복구
	if(!IsInCover)
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

	if(GetWorld())
		UGameplayStatics::PlaySound2D(GetWorld(), MeleeAttackSound);
}

void AKannaCharacter::Fire() // 여기서는 상태 전환, 애니메이션만 재생
{
	if (ActionState != EActionState::EAS_Aiming) return; // 조준 중일 때만 사격 가능
	if (CurrentWeapon == nullptr) return;
		
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();

	FVector StartPoint = ViewCamera->GetComponentLocation() + ViewCamera->GetForwardVector() * 100;
	FVector Direction = ViewCamera->GetForwardVector();

	if (CurrentWeapon && CurrentWeapon->IsShootable()) // 널체크 -> 발사 가능 확인
	{
		CurrentWeapon->Fire(StartPoint, Direction); // 총의 발사는 인터페이스에 delegate

		if (AnimInstance && FireMontage)
		{
			AnimInstance->Montage_Play(FireMontage);
			SpreadCrosshair();
		}
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
		CurrentWeapon->Reload(); // 총의 재장전을 인터페이스에 delegate
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
		WallTrace(); // 엄폐물이 있는지 탐지
	}
}

void AKannaCharacter::WallTrace()
{
	//낮은 엄폐물 탐지
	FVector LowStart = GetActorLocation();
	FVector LowEnd = GetActorLocation() + GetActorForwardVector() * 100.f;
	//높은 엄폐물 탐지
	FVector HighStart = GetActorLocation() + FVector(0.f,0.f,80.f);
	FVector HighEnd = HighStart + GetActorForwardVector() * 100.f;

	if (GetWorld())
	{
		FHitResult HitResult;

		FCollisionQueryParams CollisionParameters; // 트레이싱이 자기자신은 무시하도록
		CollisionParameters.AddIgnoredActor(this);

		//높은 엄폐물 탐지 시도
		if (GetWorld()->LineTraceSingleByChannel(HitResult, HighStart, HighEnd, ECC_GameTraceChannel1, CollisionParameters))
		{
			DrawDebugLine(GetWorld(), HighStart, HighEnd, FColor(255,0,0), true, 10.f, 0, 5.f);
			StartCover(HitResult.Normal, false);
		}
		//낮은 엄폐물 탐지 시도
		else if (GetWorld()->LineTraceSingleByChannel(HitResult, LowStart, LowEnd, ECC_GameTraceChannel1, CollisionParameters))
		{
			DrawDebugLine(GetWorld(), LowStart, LowEnd, FColor(255, 0, 0), true, 10.f, 0, 5.f);
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

	//현재 PlaneConstraintNormal은 플레이어의 이동 제한 평면의 법선벡터이다. 이는 벽면에서 바깥쪽으로 가는 방향이다.
	//원하는 벡터는 캐릭터가 벽을 향하는 방향의 벡터이므로, -1을 곱해준다.
	FVector WallDirection = Movement->GetPlaneConstraintNormal() * (-1.f);

	CheckLeftRightHit(WallDirection, ActorLocation, HitResult, CollisionParameters);

	//MoveActionBinding은 이동 인풋 값을 가져다쓰기 위해 만든 것이다. 이에 대한 설명도 후술할 것이다.
	FVector2D MoveVector = MoveActionBinding->GetValue().Get<FVector2D>();
	//PlayerRightVector는 카메라로 보는 시점 기준 오른쪽 방향 벡터이다.
	FVector PlayerRightVector = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotator(0.f, 0.f, GetControlRotation().Yaw));

	//왼쪽, 오른쪽 둘다 엄폐 공간이 남아있다면, 어느쪽이든 갈 수 있다.
	if (LeftHit && RightHit)
	{
		if (MoveVector.X != 0.f) //좌우 이동이 0이 아니라면
		{
			//앞에 엄폐물이 있는지 라인트레이싱으로 한번 더 확인하고, 법평면 제한을 한번 더 걸어준 뒤, AddMovementInput을 통해 이동시킨다.
			// 제한을 다시 거는 이유는, 실린더 모양 엄폐물에서도 좌우이동이 자연스럽게 되도록 하기 위해서이다.
			if (GetWorld()->LineTraceSingleByChannel(
					HitResult, 
					ActorLocation, 
					ActorLocation + WallDirection * 200.f,
					ECollisionChannel::ECC_GameTraceChannel1, 
					CollisionParameters)
				)
			{
				Movement->SetPlaneConstraintNormal(HitResult.Normal);

				AddMovementInput(PlayerRightVector, MoveVector.X);
			}

		}
	}
	else // 엄폐공간이 좌우 모두 자유롭게 남아있지 않은 경우
	{
		float MovementScale; //필요에 따라 0이 될 수도 있다.

		//오른쪽 공간있고, 오른쪽 이동 인풋이 들어왔다면 스케일을 그대로 해준다.
		if (RightHit && MoveVector.X > 0)
		{
			MovementScale = MoveVector.X;
		}
		//왼쪽도 마찬가지
		else if (LeftHit && MoveVector.X < 0)
		{
			MovementScale = MoveVector.X;
		}
		// 남은 공간의 위치와 이동 방향이 어긋날 때는 MovementScale를 0으로 하여 이동하지 못하게한다.
		else
		{
			MovementScale = 0;
		}
		// 계산된 MovementScale에 따라 캐릭터를 이동시킨다.
		AddMovementInput(PlayerRightVector, MovementScale);
	}

	if ((MoveVector.X > 0 && !IsCameraAtRight) || (MoveVector.X < 0 && IsCameraAtRight))
	{
		SwitchCameraPos();
	}
}

void AKannaCharacter::CheckLeftRightHit(FVector& WallDirection, FVector& ActorLocation, FHitResult& HitResult, FCollisionQueryParams& CollisionParameters)
{
	//Right Vector는 벽면을 바라보고 섰을 때 오른쪽 방향을 나타낸다. 먼저 Rotator를 만들고, 거기서 벡터를 뽑아낸다.
	//변수명을 RightRotator로 했는데, RightVector를 뽑아내기 위한 것이어서 그렇지 실제 방향은 WallDirection이다.
	//MakeRotFromX는 X축에만 기반하여 Rotator를 만든다는 의미이다.
	FRotator RightRotator = UKismetMathLibrary::MakeRotFromX(WallDirection);
	FVector RightVector = UKismetMathLibrary::GetRightVector(RightRotator);

	//캐릭터의 살짝 우측에서 벽 방향으로 라인트레이싱을 수행할 것이다.
	FVector RightStart = ActorLocation + RightVector * 45.f;
	FVector RightEnd = RightStart + WallDirection * 200.f;

	//라인트레이싱이 맞았다면 RightHit이 true
	RightHit = GetWorld()->LineTraceSingleByChannel
	(HitResult,
		RightStart,
		RightEnd,
		ECollisionChannel::ECC_GameTraceChannel1,
		CollisionParameters);

	//이번엔 같은 작업을 왼쪽에서 할 것이다.
	//WallDirection의 반대 방향 Rotator를 만들고, 거기서 RightVector를 뽑아내면 그건 캐릭터 기준 왼쪽이된다.
	FRotator LeftRotator = UKismetMathLibrary::MakeRotFromX(WallDirection * (-1.f));
	FVector LeftVector = UKismetMathLibrary::GetRightVector(LeftRotator);

	FVector LeftStart = ActorLocation + LeftVector * 45.f;
	FVector LeftEnd = LeftStart + WallDirection * 200.f;

	//Set Left Hit
	LeftHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		LeftStart,
		LeftEnd,
		ECollisionChannel::ECC_GameTraceChannel1,
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

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	//GetCharacterMovement()->RotationRate.Yaw = 500.f;
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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Interact);
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AKannaCharacter::SwitchWeapon);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Attack);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Roll);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Fire);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Reload);
		EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Triggered, this, &AKannaCharacter::TakeCover);
		EnhancedInputComponent->BindAction(SwitchCameraAction, ETriggerEvent::Triggered, this, &AKannaCharacter::SwitchCameraPos);
	}
}
