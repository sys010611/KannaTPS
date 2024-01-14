// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/KannaTPSOverlay.h"
#include "Components/TextBlock.h"

void UKannaTPSOverlay::SetCurrentAmmoText(int32 Amount)
{
	if (CurrentAmmoText)
	{
		FString String = FString::FromInt(Amount);
		CurrentAmmoText->SetText(FText::FromString(String));
	}
}

void UKannaTPSOverlay::SetTotalAmmoText(int32 Amount)
{
	if (TotalAmmoText)
	{
		FString String = FString::FromInt(Amount);
		TotalAmmoText->SetText(FText::FromString(String));
	}
}

void UKannaTPSOverlay::HideAmmoText()
{
	CurrentAmmoText->SetVisibility(ESlateVisibility::Hidden);
	TotalAmmoText->SetVisibility(ESlateVisibility::Hidden);
	AmmoTextSlash->SetVisibility(ESlateVisibility::Hidden);
}

void UKannaTPSOverlay::ShowAmmoText()
{
	CurrentAmmoText->SetVisibility(ESlateVisibility::Visible);
	TotalAmmoText->SetVisibility(ESlateVisibility::Visible);
	AmmoTextSlash->SetVisibility(ESlateVisibility::Visible);
}