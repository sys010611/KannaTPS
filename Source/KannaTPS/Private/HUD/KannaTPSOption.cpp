// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/KannaTPSOption.h"
#include "Others/KannaTPSSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Managers/GameManager.h"
#include "Character/KannaCharacter.h"

void UKannaTPSOption::InitSettings(TSubclassOf<USaveGame> SaveGameClass, USoundMix* InSoundMix, USoundClass* InSoundBGMClass, USoundClass* InSoundSFXClass)
{
	if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
	{
		GM->SoundBGMClass = InSoundBGMClass;
		GM->SoundSFXClass = InSoundSFXClass;
		GM->SoundMix = InSoundMix;
	}

	if (UGameplayStatics::DoesSaveGameExist("DefaultSaveSlot", 0) == false)
	{
		if (UKannaTPSSaveGame* SaveGameInstance = Cast<UKannaTPSSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass)))
		{
			BGMSlider->SetValue(SaveGameInstance->BGMValue);
			BGMValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(SaveGameInstance->BGMValue * 100)) + TEXT("%")));

			SFXSlider->SetValue(SaveGameInstance->SFXValue);
			SFXValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(SaveGameInstance->SFXValue * 100)) + TEXT("%")));

			MouseSensitivitySlider->SetValue(SaveGameInstance->MouseSensitivity);
			MouseSensitivityValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(SaveGameInstance->MouseSensitivity * 100)) + TEXT("%")));

			UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DefaultSaveSlot", 0);
		}
	}

	if (UKannaTPSSaveGame* SaveGameInstance = Cast<UKannaTPSSaveGame>(UGameplayStatics::LoadGameFromSlot("DefaultSaveSlot", 0)))
	{
		BGMSlider->SetValue(SaveGameInstance->BGMValue);
		ChangeSound(InSoundMix, InSoundBGMClass, SaveGameInstance->BGMValue, "BGM");

		SFXSlider->SetValue(SaveGameInstance->SFXValue);
		ChangeSound(InSoundMix, InSoundSFXClass, SaveGameInstance->SFXValue, "SFX");
	} // Set Sound Settings
}

void UKannaTPSOption::ChangeSound(USoundMix* InSoundMix, USoundClass* InSoundClass, float Volume, FString Mode)
{
	if (UKannaTPSSaveGame* SaveGameInstance = Cast<UKannaTPSSaveGame>(UGameplayStatics::LoadGameFromSlot("DefaultSaveSlot", 0)))
	{
		if (Mode == "BGM")
		{
			SaveGameInstance->BGMValue = Volume;
			BGMValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Volume * 100)) + TEXT("%")));

			if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DefaultSaveSlot", 0))
			{
				UGameplayStatics::SetSoundMixClassOverride(GetWorld(), InSoundMix, InSoundClass, Volume, 1, 0);

				if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
					GM->BGMVolume = Volume;				
			} // Save Sound Settings
		}
		else
		{
			SaveGameInstance->SFXValue = Volume;
			SFXValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Volume * 100)) + TEXT("%")));

			if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DefaultSaveSlot", 0))
			{
				UGameplayStatics::SetSoundMixClassOverride(GetWorld(), InSoundMix, InSoundClass, Volume, 1, 0);

				if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
					GM->SFXVolume = Volume;
			} // Save Sound Settings
		}
	}
}


void UKannaTPSOption::ChangeMouseSensitivity(float Value)
{
	if (UKannaTPSSaveGame* SaveGameInstance = Cast<UKannaTPSSaveGame>(UGameplayStatics::LoadGameFromSlot("DefaultSaveSlot", 0)))
	{
		SaveGameInstance->MouseSensitivity = Value;
		MouseSensitivityValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Value * 100))));

		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DefaultSaveSlot", 0))
		{
			if (UGameManager* GM = GetGameInstance()->GetSubsystem<UGameManager>())
			{
				if(AKannaCharacter* Chr = GM->KannaCharacter)
					Chr->SetMouseSensitivity(Value);
			}
		} 
	}
}