// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterStates.h"
#include "KannaAnimInstance.generated.h"

class AKannaCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class KANNATPS_API UKannaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	AKannaCharacter* KannaCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* KannaCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Movement | Character State")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Movement | Character State")
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float CharacterSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float CharacterDirection;

	UPROPERTY(BlueprintReadonly)
	float Pitch;
};
