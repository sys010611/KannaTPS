// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicator.generated.h"

/**
 * 
 */
UCLASS()
class KANNATPS_API UDamageIndicator : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	AActor* Causer;

	UPROPERTY(BlueprintReadWrite)
	APawn* KannaCharacter;

	void PlayFadeAnim();

	UFUNCTION()
	void HideWidget();

protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FadeAnim;

private:
	FWidgetAnimationDynamicEvent EndDelegate;



};
