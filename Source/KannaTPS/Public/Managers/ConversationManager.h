// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ConversationManager.generated.h"

class UConversation;

/**
 * 
 */
UCLASS()
class KANNATPS_API UConversationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetConversation(const FString& Speaker, const FString& Content);

	void SetConversationWidget(UConversation* Widget);

private:
	UPROPERTY()
	UConversation* ConversationWidget;
};
