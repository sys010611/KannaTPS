// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GameManager.h"

// Change the volume of the default SoundClass
void UGameManager::ChangeDefaultVolume(float volume)
{
	const FSoftObjectPath DefaultSoundClassName = GetDefault<UAudioSettings>()->DefaultSoundClassName;

	if (!DefaultSoundClassName.IsValid())
		return;

	USoundClass* masterSoundClass = LoadObject<USoundClass>(nullptr, *DefaultSoundClassName.ToString());

	if (masterSoundClass == nullptr)
		return;

	masterSoundClass->Properties.Volume = volume;
}

