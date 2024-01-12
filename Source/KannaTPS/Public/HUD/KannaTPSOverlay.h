// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KannaTPSOverlay.generated.h"

class UTextBlock;

UCLASS()
class KANNATPS_API UKannaTPSOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCurrentAmmoText(int32 amount);

	void SetTotalAmmoText(int32 amount);

	void HideAmmoText();

	void ShowAmmoText();
	
private:
	// meta = BindWidget은 블루프린트에서의 변수와 연동시키겠다는 의미이다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoTextSlash;
};
