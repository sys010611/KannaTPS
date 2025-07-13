// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManager.generated.h"

class AEnemy;
class AKannaCharacter;
class USaveGame;

UCLASS()
class KANNATPS_API UGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	FTimespan GetPlayTime();

	UFUNCTION(BlueprintCallable)
	void StopBGM();

	void SetKannaDamageable();

	void Alert();

	UFUNCTION(BlueprintCallable)
	bool CheckIfCleared();

	UFUNCTION(BlueprintCallable)
	void Mute();

	UFUNCTION(BlueprintCallable)
	void Unmute();

	UPROPERTY(BlueprintReadWrite)
	TArray<int> RemainingEnemyCount;

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

	USoundMix* SoundMix;
	USoundClass* SoundBGMClass;
	USoundClass* SoundSFXClass;

	float BGMVolume;
	float SFXVolume;

	UPROPERTY(BlueprintReadWrite)
	int CurrentFloor;
};
