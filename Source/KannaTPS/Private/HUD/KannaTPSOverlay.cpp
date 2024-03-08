// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/KannaTPSOverlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UKannaTPSOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltatime)
{
	ExNumberText->SetText(FText::AsNumber((int32)(ExGauge->GetPercent() * 10)));
	TickActionsAndAnimation(InDeltatime);
}

void UKannaTPSOverlay::HideOverlay()
{
	SetRenderOpacity(0.f);
}

void UKannaTPSOverlay::ShowOverlay()
{
	SetRenderOpacity(1.f);
}

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

void UKannaTPSOverlay::SetExGaugePercent(float Percent)
{
	ExGauge->SetPercent(Percent);
}

void UKannaTPSOverlay::SetDeadScreen()
{
	DeadScreen->SetVisibility(ESlateVisibility::Visible);
}

void UKannaTPSOverlay::ShowHitMarker()
{
	HitMarker->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(HitMarkerOffHandle, this, &UKannaTPSOverlay::HideHitMarker, 0.07f, false);
}

void UKannaTPSOverlay::HideHitMarker()
{
	HitMarker->SetVisibility(ESlateVisibility::Hidden);
}

void UKannaTPSOverlay::ShowOption()
{
	WBP_Option->SetVisibility(ESlateVisibility::Visible);
}
