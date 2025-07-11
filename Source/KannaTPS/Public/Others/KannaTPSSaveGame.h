// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KannaTPSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KANNATPS_API UKannaTPSSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UKannaTPSSaveGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BGMValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SFXValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivity;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

};
