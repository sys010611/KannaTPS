// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KannaCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Animation/AnimMontage.h"

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

	CapsuleDefaultHalfHeight = 88.f;

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
}

void AKannaCharacter::Move(const FInputActionValue& Value)
{
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
	if(CharacterState == ECharacterState::ECS_Unarmed) return; //비무장 상태일 시 Action State 바꾸지 않음, 카메라만 줌 인

	ActionState = EActionState::EAS_Aiming;

	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 조준 중에는 이동속도 감소
	GetCharacterMovement()->bOrientRotationToMovement = false; // 캐릭터가 방향키에 따라 회전하지 않음 (조준 방향 유지)
}

void AKannaCharacter::ReleaseAim()
{
	ActionState = EActionState::EAS_Neutral;

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
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
	if(CharacterState == ECharacterState::ECS_ArmedWithPistol)
		CharacterState = ECharacterState::ECS_Unarmed;
	else if (CharacterState == ECharacterState::ECS_Unarmed)
		CharacterState = ECharacterState::ECS_ArmedWithPistol;
}

void AKannaCharacter::Attack()
{
	if (ActionState == EActionState::EAS_Neutral) //근접 공격은 중립 상태에서만 가능
	{
		GetCharacterMovement()->DisableMovement(); //공격 중에는 움직이지 않도록

		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AKannaCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Neutral;
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking; //공격 완료 후 Movement Mode 초기화 
}

void AKannaCharacter::Roll()
{
	if (ActionState == EActionState::EAS_Neutral) //구르기는 중립 상태에서만 가능
	{
		PlayRollMontage();
		ActionState = EActionState::EAS_Rolling;
		
		OnRollStart(); // 카메라 워킹 이벤트 Invoke
	}

	//캡슐 콜라이더 크기 반으로 줄이기
	Crouch();
	//SpringArm->SocketOffset = FVector(0.f, 50.f, 60.f);
}

void AKannaCharacter::RollEnd()
{
	ActionState = EActionState::EAS_Neutral;

	//캡슐 콜라이더 크기 원상복구
	UnCrouch();
	//SpringArm->TargetArmLength = SpringArmDefaultLength;
	//SpringArm->SocketOffset = SpringArmDefaultOffset;
}

void AKannaCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage) //null check
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 RandNum = FMath::RandRange(1, 2);
		FName SectionName = FName();

		switch (RandNum)
		{
		case 1:
			SectionName = FName("Attack1");
			break;
		case 2:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
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

// Called every frame
void AKannaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKannaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Look);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AKannaCharacter::ReleaseAim);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Interact);
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AKannaCharacter::SwitchWeapon);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Attack);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AKannaCharacter::Roll);
	}
}
