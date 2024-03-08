// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManager.generated.h"

class AEnemy;
class AKannaCharacter;


UCLASS()
class KANNATPS_API UGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ChangeDefaultVolume(float volume);

	UFUNCTION(BlueprintCallable)
	void ChangeMouseSensitivity(float Value);

	UFUNCTION(BlueprintCallable)
	FTimespan GetPlayTime();

	UFUNCTION(BlueprintCallable)
	void StopBGM();

	void SetKannaDamageable();

	void Alert();

	UFUNCTION(BlueprintCallable)
	bool CheckIfCleared();

	UPROPERTY(BlueprintReadOnly)
	TArray<AEnemy*> ActiveEnemies;

	UPROPERTY(BlueprintReadOnly)
	bool IsAlerted;

	UPROPERTY(BlueprintReadWrite)
	bool IsAwared;

	AKannaCharacter* KannaCharacter;

	float MouseSensitivity = 0.65f;

	UPROPERTY(BlueprintReadWrite)
	FDateTime StartTime;
	UPROPERTY(BlueprintReadWrite)
	FDateTime EndTime;
};
