// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Conversation.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class KANNATPS_API UConversation : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override; // BeginPlay と同じ。

	void SetConversation();
	void SetMessage();
	void GetConversation(const TPair<FString, FString>& Content);
	void GetMessage(const FString& Content);
	FORCEINLINE bool CheckIfConversationEnded() { return ConversationQueue.IsEmpty(); };

protected:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FadeAnim;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* MessageFadeAnim;

private:
	void SetContentAsSubstring(); // 一文字ずつ増やして表示する関数。
	//void PlayFadeAnim(); // アニメーション再生関数


	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Column;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ContentText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;

	// 現在表示中のセリフ（全文 / 一部）。
	FString FullContent;
	FString CurrentContent;

	FTimerHandle TypewriterTimerHandle; // 一文字ずつ表示するためのタイマー
	FTimerHandle ClearContentHandle; // セリフ表示後のフェードアウト用タイマー
	FTimerHandle ClearMessageHandle; // 
	FTimerHandle ResumeConversationHandle;
	FTimerHandle ResumeMessageHandle;

	TQueue<TPair<FString, FString>> ConversationQueue;
	TQueue<FString> MessageQueue;
	//TArray<FString> MessageArray;
};