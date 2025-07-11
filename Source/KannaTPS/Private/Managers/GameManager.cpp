// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KannaCharacter.h"
#include "Managers/ConversationManager.h"
#include "Sound/SoundClass.h"


FTimespan UGameManager::GetPlayTime()
{
	return EndTime - StartTime;
}

void UGameManager::ClearActiveEnemies()
{
	ActiveEnemies.Reset();
}

void UGameManager::StopBGM()
{
	if(KannaCharacter)
		KannaCharacter->StopBGM();
}

void UGameManager::SetKannaDamageable()
{
	KannaCharacter->SetCanBeDamaged(true);

	GetGameInstance()->GetSubsystem<UConversationManager>()->
		SetConversation(TEXT("PMC 지휘관"), 
		TEXT("전 병력, 현 시간 부로 HD탄 사용을 허가한다."));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			GetGameInstance()->GetSubsystem<UConversationManager>()->SetMessage(TEXT("지나치게 공격받을 시 헤일로가 파괴됩니다."));
		}, 5.f, false);
}

void UGameManager::Alert()
{
	IsAlerted = true;

	FTimerHandle AlertHandle;
	GetWorld()->GetTimerManager().SetTimer(AlertHandle, [this]()
		{
			GetWorld()->GetGameInstance()->GetSubsystem<UConversationManager>()->SetConversation
			(
				TEXT("PMC 병사"), TEXT("공안국장이 배신했다! 공격 개시!")
			);
		}, 5.f, false);

	FTimerHandle DamageableHandle;
	GetWorld()->GetTimerManager().SetTimer(DamageableHandle, this, &UGameManager::SetKannaDamageable, 30.f, false);

	KannaCharacter->PlayBGM();
}

bool UGameManager::CheckIfCleared()
{
	return ActiveEnemies.Num() == 0;
}

void UGameManager::Mute()
{
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SoundBGMClass, 0, 1, 0);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SoundBGMClass, 0, 1, 0);
}

void UGameManager::Unmute()
{
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SoundBGMClass, BGMVolume, 1, 0);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SoundBGMClass, SFXVolume, 1, 0);
}

