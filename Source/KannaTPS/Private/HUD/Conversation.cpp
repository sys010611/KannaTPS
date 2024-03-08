// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Conversation.h"
#include "Managers/ConversationManager.h"
#include "Components/TextBlock.h"

void UConversation::NativeConstruct()
{
	// 자기 자신을 UConversationManager에 등록한다.
	GetGameInstance()->GetSubsystem<UConversationManager>()->SetConversationWidget(this);
}

void UConversation::SetConversation()
{
	// 큐에서 하나 빼기
	TPair<FString, FString>* Conversation = ConversationQueue.Peek();

	if(Conversation == nullptr) return;

	FString& Speaker = Conversation->Key;
	FString& Content = Conversation->Value;


	//Content 초기화
	FullContent = "";
	CurrentContent = "";

	//타이머 초기화.
	GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

	// 마지막 대사를 출력한 참이었다면
	if (GetWorld()->GetTimerManager().IsTimerActive(ClearContentHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ClearContentHandle);//메시지 안지워지게 타이머 초기화

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {SetConversation(); }, 2.f, false); // 2초 딜레이, 재귀호출
		return;
	}

	//화자 텍스트는 한번에 설정, 내용 텍스트는 일단 비워둠.
	SpeakerText->SetText(FText::FromString(Speaker));
	ContentText->SetText(FText::GetEmpty());

	// 투명도 1
	SpeakerText->SetRenderOpacity(1.f);
	ContentText->SetRenderOpacity(1.f);
	Column->SetRenderOpacity(1.f);

	// 전체 대사 저장.
	FullContent = Content;

	// 0.05초마다 SetContentAsSubstring을 호출한다.
	GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UConversation::SetContentAsSubstring, 0.03f, true);
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

		//큐에서 하나 빼기
		ConversationQueue.Pop();

		// 큐가 비었다면
		if (ConversationQueue.IsEmpty())
		{
			// 5초 뒤 페이드아웃 애니메이션 재생
			GetWorld()->GetTimerManager().SetTimer(ClearContentHandle, [this]() {PlayAnimation(FadeAnim); }, 5.f, false);
		}
		else
		{
			//다음 대화 출력
			GetWorld()->GetTimerManager().SetTimer(ResumeConversationHandle, [this] {SetConversation(); }, 2.f, false);
		}
	}
}


void UConversation::SetMessage()
{
	// 마지막 메시지를 출력한 참이었다면
	if (GetWorld()->GetTimerManager().IsTimerActive(ClearMessageHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ClearMessageHandle);//메시지 안지워지게 타이머 초기화

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this](){SetMessage();}, 4.f, false); // 4초 딜레이, 재귀호출
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(ClearMessageHandle);

	FString* Content = MessageQueue.Peek();

	if(Content == nullptr) return;

	MessageText->SetText(FText::FromString(*Content));

	// 투명도 1
	MessageText->SetRenderOpacity(1.f);

	MessageQueue.Pop();
	if (MessageQueue.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimer(ClearMessageHandle, [this]() {PlayAnimation(MessageFadeAnim); }, 4.f, false);
	}
	else
	{
		//다음 메시지 출력
		GetWorld()->GetTimerManager().SetTimer(ResumeMessageHandle, [this] {SetMessage(); }, 4.f, false);
	}
}

void UConversation::GetConversation(const TPair<FString, FString>& Content)
{
	bool ShouldStartConversation = ConversationQueue.IsEmpty();

	ConversationQueue.Enqueue(Content);

	if(ShouldStartConversation)
		SetConversation();
}

void UConversation::GetMessage(const FString& Content)
{
	if (!MessageQueue.IsEmpty())
	{
		if (*MessageQueue.Peek() == Content)
		{
			return;
		}
	}

	bool ShouldShowMessage = MessageQueue.IsEmpty();

	MessageQueue.Enqueue(Content);

	if (ShouldShowMessage)
		SetMessage();
}

//void UConversation::PlayFadeAnim()
//{
//	PlayAnimation(FadeAnim);
//}