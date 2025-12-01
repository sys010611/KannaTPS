// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Conversation.h"
#include "Managers/ConversationManager.h"
#include "Components/TextBlock.h"

void UConversation::NativeConstruct()
{
	// 自分自身を UConversationManager に登録する。
	GetGameInstance()->GetSubsystem<UConversationManager>()->SetConversationWidget(this);
}

void UConversation::SetConversation()
{
	// キューから一つ取り出す
	TPair<FString, FString>* Conversation = ConversationQueue.Peek();

	if (Conversation == nullptr) return;

	FString& Speaker = Conversation->Key;
	FString& Content = Conversation->Value;


	// Content を初期化
	FullContent = "";
	CurrentContent = "";

	// タイマーを初期化する。
	GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

	// 直前に最後のセリフを表示したところなら
	if (GetWorld()->GetTimerManager().IsTimerActive(ClearContentHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ClearContentHandle); // メッセージが消えないようにタイマーをリセットする

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() { SetConversation(); }, 2.f, false); // 2 秒ディレイして再帰呼び出し
		return;
	}

	// 話者テキストは一度に設定し、内容テキストはいったん空にしておく。
	SpeakerText->SetText(FText::FromString(Speaker));
	ContentText->SetText(FText::GetEmpty());

	// 不透明度を 1 にする
	SpeakerText->SetRenderOpacity(1.f);
	ContentText->SetRenderOpacity(1.f);
	Column->SetRenderOpacity(1.f);

	// 全文テキストを保存する。
	FullContent = Content;

	// 0.03 秒ごとに SetContentAsSubstring を呼び出す。
	GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UConversation::SetContentAsSubstring, 0.03f, true);
}

void UConversation::SetContentAsSubstring()
{
	// 現在の内容の長さに 1 を足した値を新しい長さとする
	int CurrLength = CurrentContent.Len() + 1;

	// *** Mid は Substring と同じ役割。 ***
	CurrentContent = FullContent.Mid(0, CurrLength);
	ContentText->SetText(FText::FromString(CurrentContent));

	// 最後まで出力し終えたら
	if (CurrLength == FullContent.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle); // タイマークリア

		// キューから一つ取り出す
		ConversationQueue.Pop();

		// キューが空なら
		if (ConversationQueue.IsEmpty())
		{
			// 5 秒後にフェードアウトアニメーションを再生する
			GetWorld()->GetTimerManager().SetTimer(ClearContentHandle, [this]() { PlayAnimation(FadeAnim); }, 5.f, false);
		}
		else
		{
			// 次の会話を表示する
			GetWorld()->GetTimerManager().SetTimer(ResumeConversationHandle, [this] { SetConversation(); }, 2.f, false);
		}
	}
}


void UConversation::SetMessage()
{
	// 直前に最後のメッセージを表示したところなら
	if (GetWorld()->GetTimerManager().IsTimerActive(ClearMessageHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ClearMessageHandle); // メッセージが消えないようにタイマーをリセットする

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() { SetMessage(); }, 4.f, false); // 4 秒ディレイして再帰呼び出し
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(ClearMessageHandle);

	FString* Content = MessageQueue.Peek();

	if (Content == nullptr) return;

	MessageText->SetText(FText::FromString(*Content));

	// 不透明度を 1 にする
	MessageText->SetRenderOpacity(1.f);

	MessageQueue.Pop();
	if (MessageQueue.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimer(ClearMessageHandle, [this]() { PlayAnimation(MessageFadeAnim); }, 4.f, false);
	}
	else
	{
		// 次のメッセージを表示する
		GetWorld()->GetTimerManager().SetTimer(ResumeMessageHandle, [this] { SetMessage(); }, 4.f, false);
	}
}

void UConversation::GetConversation(const TPair<FString, FString>& Content)
{
	bool ShouldStartConversation = ConversationQueue.IsEmpty();

	ConversationQueue.Enqueue(Content);

	if (ShouldStartConversation)
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
