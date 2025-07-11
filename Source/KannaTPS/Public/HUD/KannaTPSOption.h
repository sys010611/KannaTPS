// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KannaTPSOption.generated.h"

class USlider;
class UTextBlock;

UCLASS()
class KANNATPS_API UKannaTPSOption : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitSettings(TSubclassOf<USaveGame> SaveGameClass, USoundMix* InSoundMix, USoundClass* InSoundBGMClass, USoundClass* InSoundSFXClass);
	
	UFUNCTION(BlueprintCallable)
	void ChangeSound(USoundMix* InSoundMix, USoundClass* InSoundBGMClass, float Volume, FString Mode);

	UFUNCTION(BlueprintCallable)
	void ChangeMouseSensitivity(float Value);


protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* BGMSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* SFXSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* MouseSensitivitySlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* BGMValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SFXValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MouseSensitivityValue;

private:

};
