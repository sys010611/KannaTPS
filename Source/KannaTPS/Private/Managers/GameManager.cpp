// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KannaCharacter.h"
#include "Managers/ConversationManager.h"

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

void UGameManager::SetKannaDamageable()
{
	KannaCharacter->SetCanBeDamaged(true);

	GetGameInstance()->GetSubsystem<UConversationManager>()->
		SetConversation(TEXT("PMC 지휘관"), 
		TEXT("전 병력, 현 시간 부로 헤일로 파괴탄 사용을 허가한다. 다시 한번 전파한다. 헤일로 파괴탄 사용을 허가한다."));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			GetGameInstance()->GetSubsystem<UConversationManager>()->SetMessage(TEXT("지나치게 피격당할 시 사망합니다."));
		}, 5.f, false);
}

