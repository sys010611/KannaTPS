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
	virtual void NativeConstruct() override; //BeginPlay와 같음.

	void SetConversation(const FString& Speaker, const FString& Content);
	void SetMessage(const FString& Content);

protected:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim)) // Transient 붙여야함.
	class UWidgetAnimation* FadeAnim; 

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim)) // Transient 붙여야함.
	class UWidgetAnimation* MessageFadeAnim; 

private:
	void SetContentAsSubstring(); //한 글자씩 늘려서 출력해주는 함수.
	//void PlayFadeAnim(); // 애니메이션 재생 함수


	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Column;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ContentText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;

	// 현재 출력 중인 대사의 전체 / 일부.
	FString FullContent;
	FString CurrentContent;

	FTimerHandle TypewriterTimerHandle; //한 글자씩 치기 위한 타이머
	FTimerHandle ClearContentHandle; // 대사가 끝난 뒤 페이드아웃을 위한 타이머
	FTimerHandle ClearMessageHandle; // 
};