// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManager.generated.h"

class AEnemy;


UCLASS()
class KANNATPS_API UGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void ChangeDefaultVolume(float volume);

	UPROPERTY(BlueprintReadOnly)
	TArray<AEnemy*> ActiveEnemies;
};
