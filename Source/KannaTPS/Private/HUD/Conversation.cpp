// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Conversation.h"
#include "Managers/ConversationManager.h"
#include "Components/TextBlock.h"

void UConversation::NativeConstruct()
{
	// 자기 자신을 UConversationManager에 등록한다.
	GetGameInstance()->GetSubsystem<UConversationManager>()->SetConversationWidget(this);
	SetRenderOpacity(0.f); // 초기 투명도는 0 (안보임)
}

void UConversation::SetConversation(const FString& Speaker, const FString& Content)
{
	//타이머 초기화.
	GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ClearContentHandle);

	//화자 텍스트는 한번에 설정, 내용 텍스트는 일단 비워둠.
	SpeakerText->SetText(FText::FromString(Speaker));
	ContentText->SetText(FText::GetEmpty());

	// 투명도 1
	SetRenderOpacity(1.f);

	// 전체 대사 저장.
	FullContent = Content;

	// 0.05초마다 SetContentAsSubstring을 호출한다.
	GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UConversation::SetContentAsSubstring, 0.05f, true);
}

void UConversation::SetContentAsSubstring()
{
	// 현재 내용의 길이에서 1 증가한 것을 현재 길이로
	int CurrLength = CurrentContent.Len() + 1;

	// *** Mid는 Substring과 같다. ***
	CurrentContent = FullContent.Mid(0, CurrLength);
	ContentText->SetText(FText::FromString(CurrentContent));

	// 끝까지 다 출력했다면
	if (CurrLength == FullContent.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle); // 타이머 클리어

		// 2초 뒤 페이드아웃 애니메이션 재생
		GetWorld()->GetTimerManager().SetTimer(ClearContentHandle, this, &UConversation::PlayFadeAnim, 2.f, false);
	}
}

void UConversation::PlayFadeAnim()
{
	PlayAnimation(FadeAnim);
}