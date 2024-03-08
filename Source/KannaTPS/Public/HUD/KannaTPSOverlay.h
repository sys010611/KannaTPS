// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KannaTPSOverlay.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;

UCLASS()
class KANNATPS_API UKannaTPSOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltatime) override;

	void HideOverlay();
	void ShowOverlay();

	void SetCurrentAmmoText(int32 amount);

	void SetTotalAmmoText(int32 amount);

	void HideAmmoText();

	void ShowAmmoText();

	void SetExGaugePercent(float percent);

	void SetDeadScreen();

	void ShowHitMarker();

	void HideHitMarker();

	UFUNCTION(BlueprintCallable)
	void ShowOption();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUserWidget* WBP_Option;

private:
	// meta = BindWidget은 블루프린트에서의 변수와 연동시키겠다는 의미이다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoTextSlash;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExGauge;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExNumberText;

	UPROPERTY(meta = (BindWidget))
	UImage* DeadScreen;

	UPROPERTY(meta = (BindWidget))
	UImage* HitMarker;

	FTimerHandle HitMarkerOffHandle;
};
