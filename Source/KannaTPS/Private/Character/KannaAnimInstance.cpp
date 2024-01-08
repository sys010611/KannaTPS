// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KannaAnimInstance.h"
#include "Character/KannaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UKannaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	KannaCharacter = Cast<AKannaCharacter>(TryGetPawnOwner());
	if (KannaCharacter)
	{
		KannaCharacterMovement = KannaCharacter->GetCharacterMovement();
	}
}

void UKannaAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (KannaCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(KannaCharacterMovement->Velocity);
		IsFalling = KannaCharacterMovement->IsFalling();
		IsCrouching = KannaCharacterMovement->IsCrouching();
		CharacterState = KannaCharacter->GetCharacterState();
		ActionState = KannaCharacter->GetActionState();

		CharacterSpeed = KannaCharacter->GetVelocity().Length();
		CharacterDirection = CalculateDirection(KannaCharacter->GetVelocity(), KannaCharacter->GetActorRotation());
		Pitch = KannaCharacter->GetBaseAimRotation().Pitch;
		IsInCover = KannaCharacter->GetIsInCover();
	}
}
