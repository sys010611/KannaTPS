// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/ConversationManager.h"
#include "HUD/Conversation.h"


void UConversationManager::SetConversation(const FString& Speaker, const FString& Content)
{
	ConversationWidget->GetConversation(TPair<FString, FString>(Speaker, Content));
}

void UConversationManager::SetMessage(const FString& Content)
{
	ConversationWidget->GetMessage(Content);
}

void UConversationManager::SetConversationWidget(UConversation* Widget)
{
	ConversationWidget = Widget;
}

bool UConversationManager::CheckIfConversationEnded()
{
	return ConversationWidget->CheckIfConversationEnded();
}
