// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KannaTPSHUD.generated.h"

class UKannaTPSOverlay;

UCLASS()
class KANNATPS_API AKannaTPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UKannaTPSOverlay* GetKannaTPSOverlay();

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKannaTPSOverlay> KannaTPSOverlayClass;

	UPROPERTY()
	UKannaTPSOverlay* KannaTPSOverlay;
};
